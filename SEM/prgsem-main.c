#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <termios.h> 
#include <unistd.h>               // for STDIN_FILENO

#include <pthread.h>

#include "prg_serial_nonblock.h"
#include "messages.h"
#include "event_queue.h"
#include "gui.h"
#include "computation.h"
#include "utils.h" 
#include "xwin_sdl.h"


#define SERIAL_READ_TIMOUT_MS 500 // timeout for reading from serial port
#define PIXEL_CHANGE 20            
#define C_CHANGE 0.01             // C constant change

// shared data structure
typedef struct {
    bool quit;
    int fd;                       // serial port file descriptor
    bool computing;
    bool change_img_size;
    bool change_c_bool;
    int size_change;
    double c_change;
    bool plus;
    message msg;                  // message structure 
} data_t;

data_t initData() {
    data_t ret;
    ret.quit = false, 
    ret.fd = -1,
    ret.computing = false;
    ret.change_img_size = false;
    ret.change_c_bool = false;
    ret.size_change = 0;
    ret.c_change = 0;
    ret.plus = false;
    ret.msg.data.compute.cid = 0;  // set chunk_id
    return ret;
}

pthread_mutex_t mtx;
pthread_cond_t cond;

void* input_thread(void*);
void* serial_rx_thread(void*);     // serial receive buffer
//void* compute_visualize_thread(void*);

bool send_message(data_t *data, message *msg);
void change_values(data_t *data);

// - main ---------------------------------------------------------------------
int main(int argc, char *argv[])         
{
    data_t data = initData();
    const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0";
    data.fd = serial_open(serial);

    if (data.fd == -1) {
        fprintf(stderr, "ERROR: Cannot open serial port %s\n", serial);
        exit(100);
    }

    enum { INPUT, SERIAL_RX, NUM_THREADS };
    const char *threads_names[] = { "Input", "Serial In" };

    void* (*thr_functions[])(void*) = { input_thread, serial_rx_thread };

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mtx, NULL); // initialize mutex with default attributes
    pthread_cond_init(&cond, NULL); // initialize mutex with default attributes

    call_termios(0);

    for (int i = 0; i < NUM_THREADS; ++i) {
        int r = pthread_create(&threads[i], NULL, thr_functions[i], &data);
        fprintf(stderr, "INFO: Create thread '%s' %s\n", threads_names[i], ( r == 0 ? "OK" : "FAIL") );
    }

    computation_init();
    gui_init();
    bool quit = false;     
    while (!quit) {
        event ev = queue_pop();         

        if (ev.source == EV_KEYBOARD) {   
            pthread_mutex_lock(&mtx);           
            data.msg.type = MSG_NBR;
            switch(ev.type) {                            // handle keyboard events
                case EV_GET_VERSION:           
                    data.msg.type = MSG_GET_VERSION;     // prepare packet for get version
                    break;
                case EV_QUIT:                       
                    if (data.computing)
                        data.msg.type = MSG_ABORT;       // in case of computation -> prepare packet for abort
                    quit = true;                   
                    break;
                case EV_SET_COMPUTE:
                    set_compute(&data.msg);
                    break;
                case EV_COMPUTE:;
                    enable_comp();
                    compute(&data.msg);
                    data.computing = true;
                    break;
                case EV_COMPUTE_CPU:
                    if (data.computing) {
                        fprintf(stderr, "WARN: New computation requested but it is \
                                         discarded due to an ongoing computation\n\r");
                    }
                    else {
                        enable_comp();
                        data.computing = true;
                        calc_on_pc(&data.msg);
                        data.computing = false;
                    }
                    break;
                case EV_ABORT:
                    if (!data.computing) {
                        fprintf(stderr, "WARN: Abort requested but it is not computing\n\r");
                    }
                    else {
                        fprintf(stderr, "INFO: Abort requested\r\n");
                        abort_comp();
                        data.msg.type = MSG_ABORT;       // prepare packet for abort
                        data.computing = false;
                    }
                    break;
                case EV_CHANGE_C:
                    fprintf(stderr, "\nChoose whether to enlarge or shrink the C constant\n");
                    fprintf(stderr, "\nPress '+' in order to enlarge the C by 0.01\n");
                    fprintf(stderr, "Press '-' in order to shrink the C by 0.01\n\n");
                    data.change_c_bool = true;
                    break;
                case EV_CHANGE_IMAGE_SIZE:
                    fprintf(stderr, "\nChoose whether to enlarge or shrink the image\n");
                    fprintf(stderr, "\nPress '+' in order to enlarge the image by 20 pixels\n");
                    fprintf(stderr, "Press '-' in order to shrink the image by 20 pixels\n\n");
                    data.change_img_size = true;
                    break;
                case EV_PLUS:
                    data.plus = true;
                    change_values(&data);
                    break;
                case EV_MINUS:
                    data.plus = false;              // = minus
                    change_values(&data);
                    break;
                case EV_CHANGE_DONE:                // apply the changes 
                    if (data.change_img_size) {     // case for changing the size of image
                        load_size(data.size_change);
                        change_image_size();
                        data.change_img_size = false;
                        data.size_change = 0;
                    }  
                    if (data.change_c_bool) {       // case for changing the size of image
                        change_c_constant(data.c_change);
                        data.change_c_bool = false;
                        data.c_change = 0;
                    }  
                    break;
                case EV_RESET_CHUNK:                
                    reset_chunk_id();
                    break;
                case EV_CLEAR_BUFFER:
                    clear_buffer();
                    break;
                case EV_REFRESH:                   
                    refresh_buffer();               // refresh the image
                    break;
                default:
                    break;
        }
        if (data.msg.type != MSG_NBR) {             // message has been set
            if (!send_message(&data, &data.msg)) {
                fprintf(stderr, "ERROR: send_message() does not send all bytes of the message!\n");
            }
        }
        pthread_mutex_unlock(&mtx);
      } else if (ev.source == EV_NUCLEO) {          // handle nucleo events
         if (ev.type == EV_SERIAL) {
            pthread_mutex_lock(&mtx);

            switch (ev.data.msg->type) {
                case MSG_STARTUP:                   // button has been pressed, restart 
                    {
                    char str[STARTUP_MSG_LEN+1];
                    for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
                        str[i] = ev.data.msg->data.startup.message[i];
                    }
                    str[STARTUP_MSG_LEN] = '\0';
                    data.computing = false;
                    data.msg.data.compute.cid = 0;
                    fprintf(stderr, "INFO: Nucleo restarted - '%s'\n", str);
                    }
                    break;
                case MSG_VERSION:                  // print the version of Nucleo firmware
                    if (ev.data.msg->data.version.patch > 0) {
                     fprintf(stderr, "INFO: Nucleo firmware ver. %d.%d-p%d\n", ev.data.msg->data.version.major, 
                                                                               ev.data.msg->data.version.minor, 
                                                                               ev.data.msg->data.version.patch);
                    } else {
                     fprintf(stderr, "INFO: Nucleo firmware ver. %d.%d\n", ev.data.msg->data.version.major, 
                                                                           ev.data.msg->data.version.minor);
                    }
                    break;
                case MSG_OK:
                    fprintf(stderr, "INFO: Receive ok from Nucleo\r\n");
                    break;
                case MSG_ERROR:                              // error case
                    if (data.computing)
                        data.computing = false;
                    fprintf(stderr, "WARN: Receive error from Nucleo\r\n");
                    break;
                case MSG_COMPUTE_DATA:                       // data from one pixel
                    if (!is_abort()) {
                        update_data(&(ev.data.msg->data.compute_data));
                    }
                    if (!data.computing) {
                        fprintf(stderr, "WARN: Nucleo sends new data without computing \r\n");
                    }
                    break; 
                case MSG_DONE:                                         // nucleo finished the whole computation
                    gui_refresh(false); 
                    fprintf(stderr, "INFO: Nucleo reports the computation is done computing: %d\r\n", data.computing);  
                    if (!is_done()) {                                  // I need to compute another chunk
                        ev.type = EV_COMPUTE;
                        ev.source = EV_KEYBOARD;
                        queue_push(ev);
                    }
                    if (data.computing) {
                        data.computing = false;
                    }
                    break;
                case MSG_ABORT:                                        // abort from Nucleo after canceling calculation
                    printf("got ABORT from NUCLEO\n");
                    fprintf(stderr, "INFO: Abort from Nucleo\r\n");
                    abort_comp();
                    data.computing = false;
                    break;
                default:
                    break;
            }
        if (ev.data.msg) { 
            free(ev.data.msg);                                         // free allocated msg structure
        }
        pthread_mutex_unlock(&mtx);
        } else if (ev.type == EV_QUIT) {
            quit = true;
        } else {
                    // ignore all other events
        }
      }
    } // end main quit
    queue_cleanup();                                       // cleanup all events and free allocated memory for messages.
    for (int i = 0; i < NUM_THREADS; ++i) {
        fprintf(stderr, "INFO: Call join to the thread %s\n", threads_names[i]);
        int r = pthread_join(threads[i], NULL);
        fprintf(stderr, "INFO: Joining the thread %s has been %s\n", threads_names[i], (r == 0 ? "OK" : "FAIL"));
    }
    serial_close(data.fd);
    computation_cleanup();
    gui_cleanup();
    call_termios(1);                                       // restore terminal settings
    return EXIT_SUCCESS;
}

// - function -----------------------------------------------------------------
void* input_thread(void* d)                               // thread for keyboard input management
{
    data_t *data = (data_t*)d;
    event ev = { .source = EV_KEYBOARD };    
    bool end = false;
    int c;

    while ( !end && (c = getchar())) {
        pthread_mutex_lock(&mtx);
        ev.type = EV_TYPE_NUM;
        switch(c) {
            case 'g':                                     // get version
                ev.type = EV_GET_VERSION;
                fprintf(stderr, "INFO: Get version requested\n");
                break;
            case 'q':                                     // quit program
                ev.type = EV_QUIT;
                end = true;
                data->quit = true;
                break;
            case 's':
                ev.type = EV_SET_COMPUTE;
                break;
            case 'c':
                ev.type = EV_COMPUTE_CPU;
                break;
            case 'a':                                     // abort computation
                ev.type = EV_ABORT;
                break;
            case 'r':                                     // reset chunk_id
                if (!data->computing) {                   // only when not computing
                    fprintf(stderr, "INFO: Chunk reset request\n\r");
                    ev.type = EV_RESET_CHUNK;
                }
                else 
                    fprintf(stderr, "WARN: Chunk reset request discarded, it is currently computing\n\r");
                break;
            case 'l':
                ev.type = EV_CLEAR_BUFFER;                // empty the array with iter values stores
                break;
            case 'p':
                ev.type = EV_REFRESH;                     // update the image
                break;
            case 't':                              
                ev.type = EV_CHANGE_IMAGE_SIZE;           
                break;
            case 'u':
                ev.type = EV_CHANGE_C;
                break;
            case '+':
                ev.type = EV_PLUS;                        // enlarging image or C constant
                break;
            case '-':
                ev.type = EV_MINUS;                       // decrementing image or C constant
                break;
            case 'd':
                ev.type = EV_CHANGE_DONE;                 // apply the changes
                break;
            default:                                    
                if (c >= '1' && c <= '5') {               // start computing by pressing a random num btwn 1 - 5
                    if (!data->computing) {
                        ev.type = EV_COMPUTE;
                    }
                    else
                        fprintf(stderr, "WARN: New computation requested but it is discarded due on ongoing computation\n\r");
                } 
                else {                                                // discard all other keys
                    fprintf(stderr, "ERROR: Unknown message type has been received 0x%x\n - '%c'\r", c, c);
                }
                break;
        }
        if (ev.type != EV_TYPE_NUM) {                                // new event 
            queue_push(ev);
        }
        end = end || data->quit;                                     // check for quit
        pthread_mutex_unlock(&mtx);
   }
   ev.type = EV_QUIT;
   queue_push(ev);
   fprintf(stderr, "INFO: Exit input thread %p\n", (void*)pthread_self());
   return NULL;
}


// - function -----------------------------------------------------------------
void* serial_rx_thread(void* d)                                      // thread for reading from serial port
{                                                                    // read bytes from the serial and 
    data_t *data = (data_t*)d;                                       // puts the parsed message to the queue
    uint8_t msg_buf[sizeof(message)];                                // maximal buffer for all possible messages defined in messages.h
    event ev = { .source = EV_NUCLEO, .type = EV_SERIAL, .data.msg = NULL };
    bool end = false;
    unsigned char c;
    int i = 0;
    int len;
    while (serial_getc_timeout(data->fd, SERIAL_READ_TIMOUT_MS, &c) > 0) {}; // discard garbage

    while (!end) {
        int r = serial_getc_timeout(data->fd, SERIAL_READ_TIMOUT_MS, &c);    
        if (r > 0) {                                                // character has been read
            if (i == 0) {                                           // message type
            if (get_message_size(c, &len)) {                        // message type recognized find out 
                msg_buf[i++] = c;                                   // the length to know when to stop
            } else {
                end = true;                                         // unknown message
                fprintf(stderr, "ERROR: Unknown message type has been received 0x%x\n - '%c'\r", c, c);
            }
            } else {
            msg_buf[i++] = c;
            }
            
            if (len <= i) {                                         // reading completed
                ev.data.msg = my_malloc(sizeof(message));
                pthread_mutex_lock(&mtx);
                if (!parse_message_buf(msg_buf, i, ev.data.msg) && msg_buf[0] != MSG_OK) 
                    fprintf(stderr, "ERROR: Cannot parse message type %d\n\r", msg_buf[0]);
                pthread_mutex_unlock(&mtx);

                queue_push(ev);
                i = 0; 
                for (int j = 0; j < len; j++) {                     // make buffer empty again (and America great again)
                    msg_buf[j] = '\0';
                }
            }                     
        } else if (r == 0) {    
            pthread_mutex_lock(&mtx);                               //read but nothing has been received
            if (data->quit) {
                end = true;
                fprintf(stderr, "WARN: the packet has not been received discard what has been read\n");
            }
            pthread_mutex_unlock(&mtx);
        } else {
            fprintf(stderr, "ERROR: Cannot receive data from the serial port\n");
            end = true;
        }
        pthread_mutex_lock(&mtx); 
        end = end || data->quit;                                   // check for quit
        pthread_mutex_unlock(&mtx);
    }
    ev.type = EV_QUIT;
    queue_push(ev);
    fprintf(stderr, "INFO: Exit serial_rx_thread %p\n", (void*)pthread_self());
    return NULL;
}


// - function -----------------------------------------------------------------
bool send_message(data_t *data, message *msg)           // send message to Nucleo                         
{
    uint8_t buf[sizeof(message)];
    int len;
    int size = sizeof(message);
    bool ret = false;           
    
    if (fill_message_buf(msg, buf, size, &len)) {       // move info from msg to buffer
        for (int i = 0; i < len; i++) {
            serial_putc(data->fd, buf[i]);      
        }   
        ret = true;
    }
    return ret;
}


void change_values(data_t *data)                        // change values by pressing '+' or '-'
{
    switch (data->plus)
    {
        case true:      // '+' pressed
            if (!(data->change_img_size && data->change_c_bool) && data->change_img_size) {   // change image size

                data->size_change += PIXEL_CHANGE;
                fprintf(stderr, "SIZE CHANGE BY %d PIXELS\n", data->size_change);
            }
            else if (!(data->change_img_size && data->change_c_bool) && data->change_c_bool) {// change C size
                data->c_change += C_CHANGE;  
                fprintf(stderr, "C CHANGED BY %f \n", data->c_change);
            }
            break;
        case false:     // '-' pressed
            if (!(data->change_img_size && data->change_c_bool) && data->change_img_size) {   // change image size

                data->size_change -= PIXEL_CHANGE;
                fprintf(stderr, "SIZE CHANGE BY %d PIXELS\n", data->size_change);
            }
            else if (!(data->change_img_size && data->change_c_bool) && data->change_c_bool) {// change C size
                data->c_change -= C_CHANGE; 
                fprintf(stderr, "C CHANGED BY %f \n", data->c_change);
            }
            break;
    }
}


