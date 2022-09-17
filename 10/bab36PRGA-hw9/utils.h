/*
 * Filename: utils.h
 * Date:     2022/02/06 10:23
 * Author:   Jan Faigl
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <pthread.h>

#include "messages.h"

#define HEART_RATE_MEAN_STEP 5
#define LOW_FREQ_STEP 0.05
#define HIGH_FREQ_STEP 0.05
#define LOW_HIGH_FREQ_RATIO_STEP 0.1

// shared data structure
typedef struct {
   bool quit;
   int fd_in; // read pipeline
   int fd_out; // write pipeline
   msg_set_signal signal_param;
   pthread_mutex_t mtx;
   pthread_cond_t cond;
} data_t;

void my_assert(int r, const char *fcname, int line, const char *fname);
void call_termios(int reset);

void limit_value(double *v, double min, double max);
void print_signal_param(const msg_set_signal *param);

#endif

/* end of utils.h */
