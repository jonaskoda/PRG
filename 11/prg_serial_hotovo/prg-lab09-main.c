#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <termios.h>

#include "prg_serial.h"

void call_termios(int reset)      // for no need to press enter when pressing a button on a keyboard
{
    static struct termios tio, tioOld;
    tcgetattr(STDIN_FILENO, &tio);
    if (reset) {
        tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
    } else {
        tioOld = tio; //backup 
        cfmakeraw(&tio);
        tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    }
}

#define SLEEP_TIME(num) num * 1000
#define DEFAULT_PERIOD 5
#define PERIOD_MIN 1
#define LOAD_ERROR -1
#define PERIOD_MAX 250
#define TIMEOUT 10
#define myDEVICE "/dev/ttyACM0"

void* computeThread(void*);
void* outputThread(void*);
void* inputThread(void*);
void* serialThread(void*);

typedef struct {
    int period;
    int fd;
    bool is_serial_open;
    bool quit;
    char current;
    char LED[4];
    char last_sent;
    char last_recieved;
    int ex_counter;
    int period_ex_counter;
    int period_oh_counter;
} SharedData;
SharedData initData() {
    SharedData ret;
    ret.period = 0;
    ret.quit = false;
    ret.fd = 0;
    ret.is_serial_open = false;
    ret.current = '?';
    ret.last_sent = ' ';
    ret.last_recieved = ' ';
    ret.ex_counter = 0;
    ret.period_ex_counter = 0;
    ret.period_oh_counter = 0;
    return ret;
}
pthread_mutex_t mtx;
pthread_cond_t condvar;
int main(void)
{
    SharedData data = initData();
    call_termios(0);
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&condvar, NULL);

    enum { COMPUTE, OUTPUT, INPUT, SERIAL, NUMBER_THREADS };
    void* (*thr_functions[])(void*) = { computeThread, outputThread, inputThread, serialThread };
    pthread_t threads[NUMBER_THREADS];       // array for theads
    for (int i = 0; i < NUMBER_THREADS; ++i)
        pthread_create(&threads[i], NULL, thr_functions[i], &data);
    for (int i = 0; i < NUMBER_THREADS; ++i) {                // used to wait for the thread termination
        pthread_join(threads[i], NULL);
    }
    call_termios(1);
    //printf("\rMain thread finished\n");
    return 0;
}

void* serialThread(void *v)                                  // thread for reading from a serial port aka from Nucleo
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    char last_sent;
    unsigned char c;
    int fd = serial_open(myDEVICE);
    if (fd == -1) {                                          // error case 
        pthread_mutex_lock(&mtx);       
        data->quit = true;             
        pthread_mutex_unlock(&mtx);
        q = true;
    } else {                                                 // otherwise set data to open 
        pthread_mutex_lock(&mtx);
        data->is_serial_open = true;
        data->fd = fd;
        pthread_mutex_unlock(&mtx);
    }
    //  if ((c = serial_getc(fd)) != LOAD_ERROR)  

    while (!q) {                       
        if (serial_getc_timeout(fd, TIMEOUT, &c) != LOAD_ERROR)            // reading from the serial port  
        { 
            pthread_mutex_lock(&mtx);
            q = data->quit;  
            data->current = c;
            last_sent = data->last_sent;
            switch (c)
            {
                case 'a':
                    if (last_sent == 's' || last_sent == 'e' || last_sent == '1' 
                    || last_sent == '2' || last_sent == '3' || last_sent == '4' 
                    || last_sent == '5')
                    {
                        data->last_recieved = 'a'; 
                    }
                    if (last_sent == 's')
                        strcpy(data->LED,"on");
                    else
                        strcpy(data->LED,"off");
                    break;
                case 'h':
                    if (last_sent == 'h')
                    {
                        data->last_recieved = 'h';  
                    }
                    break;
                case 'b':
                    if (last_sent == 'b')
                    {
                        strcpy(data->LED,"off");
                        data->last_recieved = 'b';
                        q = true;
                    }
                case 'x':
                    printf("%c\n", c);
                    data->ex_counter++; 
                    data->period_ex_counter++;
                    strcpy(data->LED,"on");
                    break;
                case 'o':
                    printf("%c\n", c);
                    strcpy(data->LED,"off");
                    data->period_oh_counter++;
                    break;

            }
            pthread_cond_signal(&condvar);  
            pthread_mutex_unlock(&mtx);  
        }
        else
        {
            pthread_mutex_lock(&mtx);
            data->quit = true;
            q = true;
            pthread_mutex_unlock(&mtx);  
        }
    }
    serial_close(fd);
    /*
    pthread_mutex_lock(&mtx);
    q = data->quit;  
    pthread_cond_broadcast(&condvar);
    pthread_mutex_unlock(&mtx);
    */
    //printf("\rSerial thread finished\n");
    return 0;
}

void* computeThread(void *v)             // thread for calculating an average period every 5s
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    float frequency;
    while (!q) {
        pthread_mutex_lock(&mtx);
        data->period_ex_counter = 0;
        data->period_oh_counter = 0;
        pthread_mutex_unlock(&mtx);
        sleep(DEFAULT_PERIOD);                   // computing thread sleeps for a given period, then starts computing
        pthread_mutex_lock(&mtx);
        frequency = ((float)data->period_ex_counter + data->period_oh_counter)/((float)DEFAULT_PERIOD); 
        data->period = ((int)(((float)1/frequency)*1000));
        if (data->period < 0)
        {
            data->period = 0;
        }
        q = data->quit;
        pthread_cond_signal(&condvar);     
        pthread_mutex_unlock(&mtx);
    }
    //printf("\rCompute thread finished\n");
    return 0;
}

void* outputThread(void *v)   // vlakno na vypisovani na terminal
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    while (!q) {
        pthread_mutex_lock(&mtx);
        pthread_cond_wait(&condvar, &mtx);  // Refresh output only when the user interacts with the application or the alarm is signaling the period has been passed
        //int tmp = data->counter;          // = unlock the thread and wait for signal
        //int period = data->period;
        q = data->quit;
        char LED[4];
        strcpy(LED, data->LED);
        char send = data->last_sent;
        char recieved = data->last_recieved;
        int final_period = data->period;
        int ex_counter = data->ex_counter;
        pthread_mutex_unlock(&mtx);
        if (!q) {
            printf("\rLED %3s send: '%c' received: '%c', T = %4d ms, ticker = %4d", LED, send, recieved, final_period, ex_counter);
            fflush(stdout);  
        }
    }
    //printf("\rOutput thread finished\n");
    return 0;
}

void* inputThread(void *v)                      // vlakno pro cteni z klavesnice 
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    bool qq = false;
    while (!q && !qq) {
        pthread_mutex_lock(&mtx);               // initialize the variables
        q = data->quit;
        qq = data->is_serial_open;
        pthread_mutex_unlock(&mtx);
    }
    if (qq)
        while (!q) {
            char c = getchar();                 // read from the terminal
            pthread_mutex_lock(&mtx);
            _Bool changed = true;
            switch (c) {
            case 's':
                serial_putc(data->fd,'s');      // poslat prikaz nucleu
                break;
            case 'e':
                serial_putc(data->fd,'e');
                break;
            case 'b':                           // end the nucleo program
                serial_putc(data->fd,'b');
                break;
            case 'h':
                serial_putc(data->fd,'h');
                break;    
            case 'q':                           // end the VS program
                data->quit = true;
                break;
            default:
                if (c >= '1' && c <= '5')
                {
                    serial_putc(data->fd,c);
                } 
                else
                {
                    changed = false;   
                }
            }
            if (changed)
            {
                data->last_sent = c;
                data->last_recieved = '?';
                
            }
            q = data->quit;
            pthread_cond_signal(&condvar);
            pthread_mutex_unlock(&mtx);
        }
    //printf("\rInput thread finished\n");
    return 0;
} 

 /*
            case 'p':
                if (data->period < PERIOD_MAX)
                    data->period++;
                break;
            case 'm':
                if (data->period > PERIOD_MIN)
                    data->period--;
                break;
            */

// maybe at the beginning of input thread
    /*
    if (fd == -1) {                              // error case 
        pthread_mutex_lock(&mtx);       
        data->quit = true;             
        pthread_mutex_unlock(&mtx);
        q = true;
    } else {                                     // otherwise set data to open 
        pthread_mutex_lock(&mtx);  
        qq = true;
        data->fd = fd;
        pthread_mutex_unlock(&mtx);
    }*/

   /*
            if (serial_getc_timeout(fd, 100, &cb) != -1) {
                if (c == 'x')
                { 
                    pthread_mutex_lock(&mtx);
                    strcpy(data->LED,"on");    
                    pthread_mutex_unlock(&mtx); 
                }
                else if ((c = serial_getc(fd)) == 'o')
                {
                    pthread_mutex_lock(&mtx);
                    strcpy(data->LED,"off");    
                    pthread_mutex_unlock(&mtx); 
                }  
            }
            */

