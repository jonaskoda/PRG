#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>  // for STDIN_FILENO

#define SLEEP_TIME(num) num * 1000
#define DEFAULT_TIME  100


void* compute_thread(void*);
void* output_thread(void*);
void* input_thread(void*);

typedef struct 
{
    int counter;
    _Bool quit;
} shared_data;

shared_data init_data()
{
    shared_data ret;
    ret.counter = 0;
    ret.quit = false;
    return ret; 
}
pthread_mutex_t mtx;
pthread_cond_t condvar;


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


int main(void)  // vlakna maji data a ta data nejakym zpusobem sdili, kazdou akci musime nejak zapouzdrit - mutex
{
    pthread_cond_init(&condvar, NULL);  // first is a poitner to the pthread variable
    shared_data data = init_data();
    call_termios(0);
    enum {FIRST, SECOND, THIRD, NUM_THREADS};
    void *(*thr_functions[])(void*) = {compute_thread, output_thread, input_thread};
    pthread_t thread[NUM_THREADS];  // = 4
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&thread[i], NULL, thr_functions[i], &data);  // first pointer to pthread var, 2 atrribute, 3 functions to execute when creating a thread
    }                                                              // 4 argument that will be passed to the function
    //getchar();
    //data.quit = true;
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(thread[i], NULL);   // joinem napojit toto vlakno na hlavni vlakno, tim padem hlavni vlakno nemuze skoncit driv, nez moje vlakno
    }                                    // dochazelo k uniku pameti, protoze vlakno nebylo uvolnene
    call_termios(1);
    printf("\rMain thread finished\n");
    return 0;
}

void *compute_thread(void *v) // obecna deklarace vlakna, obecne neco muze vracet
{
    shared_data *data = (shared_data*) v;   // podobne HW06!!! pretypovani void *v na shared_data
    bool q = data->quit;
    while (!q) {
       usleep(SLEEP_TIME(DEFAULT_TIME));
       pthread_mutex_lock(&mtx);
       data->counter += 1;
       //printf("\rCounter %10i", data->counter);
       //fflush(stdout);
       q = data->quit;
       pthread_cond_signal(&condvar);
       pthread_mutex_unlock(&mtx);
    }
    printf("\rMain thread finished\n");
    return 0;
}

void *output_thread(void *v) 
{
   shared_data *data = (shared_data*) v;
   bool q = false;
   while (!q) {
        pthread_mutex_lock(&mtx);
        pthread_cond_wait(&condvar, &mtx);
        int tmp = data->counter;
        pthread_mutex_unlock(&mtx);
        printf("\rCounter %10i", tmp);
        fflush(stdout);
        q = data->quit;
   }
   return 0; 
}

void *input_thread(void *v) 
{
   shared_data *data = (shared_data*) v;
   bool q = false;
   while (!q) {
       char c = getchar();
       if (c = 'q')
       {
        pthread_mutex_lock(&mtx);
        data->quit=true;
        pthread_cond_broadcast(&condvar);
        pthread_mutex_unlock(&mtx);
        q = data->quit;
        }
    }
    printf("\rinput thread finished\n");
    return 0; 
}
