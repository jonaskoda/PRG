/*
 * Filename: input_key_thrd.c
 * Date:     2022/04/13 12:45
 * Author:   Jan Faigl
 */

#include <stdio.h>

#include <pthread.h>

#include "messages.h"
#include "event_queue.h"
#include "utils.h"

#include "input_key_thrd.h"

// - function -----------------------------------------------------------------
void* input_keyboard_thread(void* d)
{
   data_t *data = (data_t*)d;
   int c;
   bool end = false;
   event ev = { .source = EV_KEYBOARD };
   while ( !end && (c = getchar())) {
      ev.type = EV_TYPE_NUM;
      pthread_mutex_lock(&data->mtx);
      switch(c) {
	 case 'g': // get version
	    ev.type = EV_GET_VERSION;
	    break;
	 case 's':
	    ev.type = EV_SET_SIGNAL;
	    break;
	 case '1':
	    data->signal_param.heart_rate_mean -= HEART_RATE_MEAN_STEP;
	    limit_value(&(data->signal_param.heart_rate_mean), HEART_RATE_MEAN_MIN, HEART_RATE_MEAN_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '2':
	    data->signal_param.heart_rate_mean += HEART_RATE_MEAN_STEP;
	    limit_value(&(data->signal_param.heart_rate_mean), HEART_RATE_MEAN_MIN, HEART_RATE_MEAN_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '3':
	    data->signal_param.low_frequency -= LOW_FREQ_STEP;
	    limit_value(&(data->signal_param.low_frequency), LOW_FREQ_MIN, LOW_FREQ_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '4':
	    data->signal_param.low_frequency += LOW_FREQ_STEP;
	    limit_value(&(data->signal_param.low_frequency), LOW_FREQ_MIN, LOW_FREQ_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '5':
	    data->signal_param.high_frequency -= HIGH_FREQ_STEP;
	    limit_value(&(data->signal_param.high_frequency), HIGH_FREQ_MIN, HIGH_FREQ_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '6':
	    data->signal_param.high_frequency += HIGH_FREQ_STEP;
	    limit_value(&(data->signal_param.high_frequency), HIGH_FREQ_MIN, HIGH_FREQ_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '7':
	    data->signal_param.low_high_frequency_ratio -= LOW_HIGH_FREQ_RATIO_STEP;
	    limit_value(&(data->signal_param.low_high_frequency_ratio), LOW_HIGH_FREQ_RATIO_MIN, LOW_HIGH_FREQ_RATIO_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case '8':
	    data->signal_param.low_high_frequency_ratio += LOW_HIGH_FREQ_RATIO_STEP;
	    limit_value(&(data->signal_param.low_high_frequency_ratio), LOW_HIGH_FREQ_RATIO_MIN, LOW_HIGH_FREQ_RATIO_MAX);
	    print_signal_param(&(data->signal_param));
	    break;
	 case 'r':
	    ev.type = EV_RECEIVE_SIGNAL_SINGLE;
	    break;
	 case 'b': 
	    ev.type = EV_RECEIVE_SIGNAL_RANGE;
	    break;
	 case 'c':
	    ev.type = EV_CLEAR_PLOT;
	    break;
	 case 'a':
	    ev.type = EV_ABORT;
	    break;
	 case 'q':
	    end = true;
	    break;
	 default: // discard all other keys
	    break;
      } // end switch
      pthread_mutex_unlock(&data->mtx);
      if (ev.type != EV_TYPE_NUM) { // new event 
	 queue_push(ev);
      }
      pthread_mutex_lock(&data->mtx);
      end = end || data->quit; // check for quit
      pthread_mutex_unlock(&data->mtx);
   }
   ev.type = EV_QUIT;
   queue_push(ev);
#ifdef __linux__
   fprintf(stderr, "INFO: Exit input thead %ld\n", pthread_self());
#else
   fprintf(stderr, "INFO: Exit input thead %p\n", pthread_self());
#endif
   return NULL;
}

/* end of input_key_thrd.c */
