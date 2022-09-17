#include "mbed.h"
#include "messages.h"

DigitalOut myled(LED1);

Serial serial(SERIAL_TX, SERIAL_RX);
Ticker ticker;

void Tx_interrupt();
void Rx_interrupt();
bool send_buffer(const uint8_t* msg, unsigned int size);
bool receive_message(uint8_t *msg_buf, int size, int *len);
bool send_message(const message *msg, uint8_t *buf, int size);
 
#define BUF_SIZE 255
#define COMPUTE_TIME 0.1
#define VERSION_MAJOR 0
#define VERSION_MINOR 9
#define VERSION_PATCH 0
 
char tx_buffer[BUF_SIZE];
char rx_buffer[BUF_SIZE];
 
// pointers to the circular buffers
volatile int tx_in = 0;
volatile int tx_out = 0;
volatile int rx_in = 0;
volatile int rx_out = 0;

#define MESSAGE_SIZE sizeof(message)

InterruptIn button_event(USER_BUTTON);
volatile bool abort_request = false;

void tick() {
    myled = !myled;
}

void button()
{
    abort_request = true;
}


int main() {
    serial.baud(115200);
    serial.attach(&Rx_interrupt, Serial::RxIrq); // attach interrupt handler to receive data
    serial.attach(&Tx_interrupt, Serial::TxIrq); // attach interrupt handler to transmit data

    for (int i = 0; i < 10; i++) {
        myled = !myled;
        wait(0.15);
    }   
    while (serial.readable())
        serial.getc();
//    serial.putc('i');
    struct {
       uint16_t chunk_id;
       uint16_t nbr_tasks;
       uint16_t task_id;
    } compute_data = { 0, 0, 0 };    
    
    msg_version VERSION = { .major = VERSION_MAJOR, .minor = VERSION_MINOR, .patch = VERSION_PATCH };
    message msg = { .type = MSG_STARTUP, .data.startup.message = {'K','O','D','I','T','J','O','N'} };
    uint8_t msg_buf[MESSAGE_SIZE];
    int msg_len;
    button_event.rise(button);

    send_message(&msg, msg_buf, MESSAGE_SIZE);

    bool computing = false;
    double period = 0.2;
    uint8_t n;
    double c_re;  // c constant real part
    double c_im;  // c constant imaginary part
    double d_re;  // increment in the x coords (in numbers)
    double d_im;  // increment in the y coords (in numbers)
    uint8_t cid;  // chunk id
    double re;    // start of the x coords for the given chunk
    double im;    //  --||--      y coords
    uint8_t n_re; // number of cells in x direction
    uint8_t n_im; // number of cells in y direction
    uint8_t iter_cnt;
    double oldZ_real, oldZ_imag, newZ_real, newZ_imag;
    int pixel;
    uint8_t i, j;

    while (1) {
        if (abort_request) {
            if (computing) {  //abort computing
                msg.type = MSG_ABORT;
                send_message(&msg, msg_buf, MESSAGE_SIZE);
                computing = false;
                abort_request = false;
                ticker.detach();
                myled = 0;
            }
        }
        if (rx_in != rx_out) { // something is in the receive buffer
            if (receive_message(msg_buf, MESSAGE_SIZE, &msg_len)) {
                if (parse_message_buf(msg_buf, msg_len, &msg)) {
                    switch(msg.type) {
                        case MSG_GET_VERSION:
                            msg.type = MSG_VERSION;
                            msg.data.version = VERSION;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                        case MSG_ABORT:
                            msg.type = MSG_OK;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            computing = false;
                            abort_request = false;
                            ticker.detach();
                            msg.type = MSG_ABORT;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            myled = 0;
                            break;
                        case MSG_SET_COMPUTE:
                            n = msg.data.set_compute.n; 
                            c_re = msg.data.set_compute.c_re;
                            c_im = msg.data.set_compute.c_im;
                            d_re = msg.data.set_compute.d_re;
                            d_im = msg.data.set_compute.d_im;
                            msg.type = MSG_OK;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                        case MSG_COMPUTE:
                            if (computing == false) {
                                ticker.attach(tick, period);
                                cid = msg.data.compute.cid;
                                re = msg.data.compute.re;
                                im = msg.data.compute.im;
                                n_re = msg.data.compute.n_re;
                                n_im = msg.data.compute.n_im;
                                i = j = 0;
                                pixel = 0;
                                computing = true;
                            }
                            msg.type = MSG_OK;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                    } // end switch
                } else { // message has not been parsed send error
                    msg.type = MSG_ERROR;
                    send_message(&msg, msg_buf, MESSAGE_SIZE);
                }
            } // end message received
        } else if (computing) {
            if ((n_re) * (n_im) > pixel) {     // base case
                
                newZ_real = re + (i * d_re);
                newZ_imag = im + (j * d_im);
                
                iter_cnt = 0;

                while(iter_cnt < n && (newZ_real * newZ_real + newZ_imag * newZ_imag) < 4)
                {
                    oldZ_real = newZ_real;
                    oldZ_imag = newZ_imag;

                    newZ_real = (oldZ_real * oldZ_real) - (oldZ_imag * oldZ_imag) + c_re;
                    newZ_imag = (2 * oldZ_real * oldZ_imag) + c_im;
                    iter_cnt++;
                }
                msg.data.compute_data.cid = cid;
                msg.data.compute_data.i_re = i;
                msg.data.compute_data.i_im = j;
                msg.data.compute_data.iter = iter_cnt;
                
                msg.type = MSG_COMPUTE_DATA;

                send_message(&msg, msg_buf, MESSAGE_SIZE);
                
                pixel++;
                if ((i + 1) % n_re == 0)         
                {
                    j++;                  // start of another row
                    i = 0;
                }
                else
                {
                    i++;                  // continue in the row
                }
            } else { //computation done
                ticker.detach();
                myled = 0;
                msg.type = MSG_DONE;
                send_message(&msg, msg_buf, MESSAGE_SIZE);
                computing = false;
            }
        } else {
            sleep(); // put the cpu to sleep mode, it will be wakeup on interrupt
        }
    } // end while (1)
}


bool send_message(const message *msg, uint8_t *buf, int size) {
    return fill_message_buf(msg, buf, MESSAGE_SIZE, &size) && send_buffer(buf, size);
}


void Tx_interrupt()
{
    // send a single byte as the interrupt is triggered on empty out buffer 
    if (tx_in != tx_out) {
        serial.putc(tx_buffer[tx_out]);
        tx_out = (tx_out + 1) % BUF_SIZE;
    } else { // buffer sent out, disable Tx interrupt
        USART2->CR1 &= ~USART_CR1_TXEIE; // disable Tx interrupt
    }
    return;
}


void Rx_interrupt()
{
    // receive bytes and stop if rx_buffer is full
    while ((serial.readable()) && (((rx_in + 1) % BUF_SIZE) != rx_out)) {
        rx_buffer[rx_in] = serial.getc();
        rx_in = (rx_in + 1) % BUF_SIZE;
    }
    return;
}


bool send_buffer(const uint8_t* msg, unsigned int size)
{
    if (!msg && size == 0) {
        return false;    // size must be > 0
    }
 
    int i = 0;
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
 
    while ( (i == 0) || i < size ) { //end reading when message has been read
        if ( ((tx_in + 1) % BUF_SIZE) == tx_out) { // needs buffer space
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for sending buffer
            USART2->CR1 |= USART_CR1_TXEIE; // enable Tx interrupt
            while (((tx_in + 1) % BUF_SIZE) == tx_out) {
                /// let interrupt routine empty the buffer
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
 
        tx_buffer[tx_in] = msg[i];
        i += 1;
        tx_in = (tx_in + 1) % BUF_SIZE;
    } // send buffer has been put to tx buffer, enable Tx interrupt for sending it out
 
    USART2->CR1 |= USART_CR1_TXEIE; // enable Tx interrupt
 
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
 
    return true;
}


bool receive_message(uint8_t *msg_buf, int size, int *len)
{
    bool ret = false;
    int i = 0;
    *len = 0; // message size
 
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
 
    while ( ((i == 0) || (i != *len)) && i < size ) {
        if (rx_in == rx_out) { // wait if buffer is empty
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for receing buffer
            while (rx_in == rx_out) { // wait of next character
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
 
        uint8_t c = rx_buffer[rx_out];
        if (i == 0) { // message type
            if (get_message_size(c, len)) { // message type recognized
                msg_buf[i++] = c;
                ret = *len <= size; // msg_buffer must be large enough
            } else {
                ret = false;
                break; // unknown message
            }
        } else {
            msg_buf[i++] = c;
        }
        rx_out = (rx_out + 1) % BUF_SIZE;
    }
 
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
 
    return ret;
}

