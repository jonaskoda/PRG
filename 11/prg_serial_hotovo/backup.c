#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>
#include <termios.h>

#include "prg_serial.h"

void call_termios(int reset)
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
#define DEFAULT_PERIOD 1
#define PERIOD_MIN 5
#define PERIOD_MAX 250
#define myDEVICE "/dev/ttyACM0"

void* computeThread(void*);
void* outputThread(void*);
void* inputThread(void*);
void* serialThread(void*);

typedef struct {
    int counter;
    int period;
    int fd;
    bool is_serial_open;
    bool quit;
    int led;
} SharedData;
SharedData initData() {
    SharedData ret;
    ret.counter = 0;
    ret.period = DEFAULT_PERIOD;
    ret.quit = false;
    ret.fd = 0;
    ret.is_serial_open = false;
    ret.led = 0;
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
    for (int i = 0; i < NUMBER_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
    call_termios(1);
    printf("\rMain thread finished, counter: - %4i\n", data.counter);
    return 0;
}

void* serialThread(void *v)             // thread for reading from a serial port aka from Nucleo
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    unsigned char cb;
    int fd = serial_open(myDEVICE);
    if (fd == -1) {                     // error case 
        pthread_mutex_lock(&mtx);       
        data->quit = true;             
        pthread_mutex_unlock(&mtx);
        q = true;
    } else {                            // otherwise set data to open 
        pthread_mutex_lock(&mtx);
        data->is_serial_open = true;
        data->fd = fd;
        pthread_mutex_unlock(&mtx);
    }
                  
    while (!q) {                        // reading from the serial port
        pthread_mutex_lock(&mtx);
        q = data->quit;  
        pthread_mutex_unlock(&mtx);    
  
        if (serial_getc_timeout(fd, 10, &cb) != -1) {
            pthread_mutex_lock(&mtx);
            data->led = serial_getc(fd);
            printf("%d\n", data->led);
            pthread_mutex_unlock(&mtx);
        }  
    }
    serial_close(fd);
    printf("\rSerial thread finished\n");
    return 0;
}

void* computeThread(void *v)             // thread for calculating an average period every 5s
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    while (!q) {
        pthread_mutex_lock(&mtx);
        int period = data->period;
        pthread_mutex_unlock(&mtx);
        sleep(period);
        pthread_mutex_lock(&mtx);
        data->counter += 1;
        q = data->quit;
        pthread_cond_signal(&condvar);     
        pthread_mutex_unlock(&mtx);
    }
    printf("\rCompute thread finished\n");
    return 0;
}

void* outputThread(void *v)   // vlakno na vypisovani na terminal
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    while (!q) {
        pthread_mutex_lock(&mtx);
        pthread_cond_wait(&condvar, &mtx);
        int tmp = data->counter;
        int period = data->period;
        q = data->quit;
        pthread_mutex_unlock(&mtx);
        if (!q) {
            printf("\rCounter %10i Period %10i", tmp, period);   // “\rLED %3s send: '%c' received: '%c', T = %4d ms, ticker = %4d”, 
            fflush(stdout);
        }
    }
    printf("\rOutput thread finished\n");
    return 0;
}

void* inputThread(void *v)  // vlakno pro cteni z klavesnice 
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    bool qq = false;
    while (!q && !qq) {
        pthread_mutex_lock(&mtx);                // initialize the variables
        q = data->quit;
        qq = data->is_serial_open;
        pthread_mutex_unlock(&mtx);
    }
    if (qq)
        while (!q) {
            char c = getchar();                 // read from the terminal
            pthread_mutex_lock(&mtx);
            switch (c) {
            case 's':
                printf("recieved command s");
                // poslat prikaz nucleu
                serial_putc(fd, 's');
                break;
            case 'e':
                printf("recieved command e");
                break;
            case 'b':                           // end the nucleo program
                printf("recieved command b");
                break;
            case 'h':
                printf("recieved command h");
                break;    
            case 'q':                           // end the VS program
                data->quit = true;
                break;
            default:
                if (c >= '1' && c <= '5')
                {
                    printf("recieved command 1-5");
                }
                else
                {
                    ;
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
            }
//            if (serial_putc(data->fd, c) == -1) ;
            q = data->quit;
            pthread_mutex_unlock(&mtx);
        }
    printf("\rInput thread finished\n");
    return 0;
} 
