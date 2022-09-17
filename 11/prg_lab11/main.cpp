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
 
char tx_buffer[BUF_SIZE];
char rx_buffer[BUF_SIZE];
 
// pointers to the circular buffers
volatile int tx_in = 0;
volatile int tx_out = 0;
volatile int rx_in = 0;
volatile int rx_out = 0;

#define MESSAGE_SIZE sizeof(message)

void tick() {
    myled = !myled;
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
    serial.putc('i');
    struct {
       uint16_t chunk_id;
       uint16_t nbr_tasks;
       uint16_t task_id;
    } compute_data = { 0, 0, 0 };    

    message msg = { .type = MSG_STARTUP, .data.startup.message = {'P','R','G','-','L','A','B','1','0'} };
    uint8_t msg_buf[MESSAGE_SIZE];
    int msg_len;

    send_message(&msg, msg_buf, MESSAGE_SIZE);
    bool abort_request = true;
    bool computing = false;
    float period = 0.2;
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
                            msg.data.version.major  = 0;
                            msg.data.version.minor  = 1;
                            msg.data.version.patch  = 0;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                        case MSG_ABORT:
                            msg.type = MSG_OK;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            computing = false;
                            abort_request = false;
                            ticker.detach();
                            myled = 0;
                            break;
                        case MSG_COMPUTE:
                            if (msg.data.compute.nbr_tasks > 0) {
                                ticker.attach(tick, period);
                                compute_data.chunk_id = msg.data.compute.chunk_id;
                                compute_data.nbr_tasks = msg.data.compute.nbr_tasks;
                                compute_data.task_id = 0; // reset the task counter
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
            if (compute_data.task_id < compute_data.nbr_tasks) {
                wait(0.3); // do some computation
                msg.type = MSG_COMPUTE_DATA;
                msg.data.compute_data.chunk_id = compute_data.chunk_id;
                msg.data.compute_data.task_id = compute_data.task_id;
                msg.data.compute_data.result = rand() % 255;
                compute_data.task_id += 1;
                send_message(&msg, msg_buf, MESSAGE_SIZE);
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
    return fill_message_buf(msg, buf, MESSAGE_SIZE, &size) &&
        send_buffer(buf, size);
}

void Tx_interrupt()
{
    // send bytes from the tx_buffer and stop if the buffer is empty
    while ((serial.writeable()) && (tx_in != tx_out)) {
        serial.putc(tx_buffer[tx_out]);
        tx_out = (tx_out + 1) % BUF_SIZE;
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
        return true;    // size must be > 0
    }
    bool ret = false;
    int i = 0;
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
    bool empty = (tx_in == tx_out);
    while ( (i == 0) || i < size ) { //end reading when message has been read
        if ( ((tx_in + 1) % BUF_SIZE) == tx_out) { // needs buffer space
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for sending buffer
            while (((tx_in + 1) % BUF_SIZE) == tx_out) {
                /// let interrupt routine empty the buffer
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
        tx_buffer[tx_in] = msg[i];
        i += 1;
        tx_in = (tx_in + 1) % BUF_SIZE;
    }
    if (empty && serial.writeable()) {
        uint8_t c = tx_buffer[tx_out];
        tx_out = (tx_out + 1) % BUF_SIZE;
        serial.putc(c); // send first character to start tx interrupts (if stopped)
    }
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
    return ret;
}

bool receive_message(uint8_t *msg_buf, int size, int *len)
{
    bool ret = false;
    int i = 0;
    *len = 0; // message size
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
    while ( (i == 0) || (i != *len) ) {
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
