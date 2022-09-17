#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>
#include <termios.h>

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
#define DEFAULT_PERIOD 100
#define PERIOD_MIN 5
#define PERIOD_MAX 250

void* computeThread(void*);
void* outputThread(void*);
void* inputThread(void*);

typedef struct {
    int counter;
    int period;
    bool quit;
} SharedData;
SharedData initData() {
    SharedData ret;
    ret.counter = 0;
    ret.period = DEFAULT_PERIOD;
    ret.quit = false;
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

    enum { COMPUTE, OUTPUT, INPUT, NUMBER_THREADS };
    void* (*thr_functions[])(void*) = { computeThread, outputThread, inputThread };
    pthread_t threads[NUMBER_THREADS];
    for (int i = 0; i < NUMBER_THREADS; ++i)
        pthread_create(&threads[i], NULL, thr_functions[i], &data);
    for (int i = 0; i < NUMBER_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
    call_termios(1);
    printf("\rMain thread finished, counter: - %4i\n", data.counter);
    return 0;
}

void* computeThread(void *v)
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    while (!q) {
        pthread_mutex_lock(&mtx);
        int period = data->period;
        pthread_mutex_unlock(&mtx);
        usleep(SLEEP_TIME(period));
        pthread_mutex_lock(&mtx);
        data->counter += 1;
        q = data->quit;
        pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mtx);
    }
    printf("\rCompute thread finished\n");
    return 0;
}

void* outputThread(void *v) 
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
            printf("\rCounter %10i Period %10i", tmp, period);
            fflush(stdout);
        }
    }
    printf("\rOutput thread finished\n");
    return 0;
}

void* inputThread(void *v) 
{
    SharedData* data = (SharedData*)v;
    bool q = false;
    while (!q) {
        char c = getchar();
        pthread_mutex_lock(&mtx);
//       int period =  data->period;
        switch (c) {
        case 'q':
            data->quit = true;
            break;
        case 'p':
            if (data->period < PERIOD_MAX)
                data->period++;
            break;
        case 'm':
            if (data->period > PERIOD_MIN)
                data->period--;
            break;
        }
//        if (period != data->period)
//            pthread_cond_broadcast(&condvar);
        q = data->quit;
        pthread_mutex_unlock(&mtx);
    }
    printf("\rInput thread finished\n");
    return 0;
} 
