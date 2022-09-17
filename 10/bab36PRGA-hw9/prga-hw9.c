/*
 * Filename: prga-hw9.c
 * Date:     2022/02/06 13:38
 * Author:   Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>

#include <assert.h>

#include <termios.h> 
#include <unistd.h>  // for STDIN_FILENO

#include <pthread.h>

#include "utils.h"
#include "messages.h"
#include "event_queue.h"
#include "xwin_otk_plot.h"

#include "prg_io_nonblock.h"

#include "input_key_thrd.h"
#include "pipe_io.h"

typedef struct {
      msg_set_signal param;
      msg_signal_data data;
      bool computing;

      const int N;

      float *signal_data;
      float *ticks;
      uint8_t cid;
      int received_data;

      bool receive_signal_range;
      bool wait_for_set_signal_ack;
      unsigned int signal_id;
      unsigned int signal_start_id;
      unsigned int signal_end_id;
      float y_min;
      float y_max;
} computation_t;

// return quit of quit requested
void handle_keyboard(event *ev, computation_t *computation, data_t *data);
void handle_pipe_in(event *ev, computation_t *computation, data_t *data);


// - main ---------------------------------------------------------------------
int main(int argc, char *argv[])
{
   data_t data = { 
      .quit = false, .fd_in = -1, .fd_out = -1,
      .signal_param.heart_rate_mean = 60.0,
      .signal_param.low_frequency = 0.1,
      .signal_param.high_frequency = 0.25,
      .signal_param.low_high_frequency_ratio = 0.5
   };

   computation_t computation = { 
      .computing = false, 
      .N = 722,
      .signal_data = NULL, .ticks = NULL, //need to be allocated later
      .cid = 0, .received_data = 0,
      .receive_signal_range = false, .signal_id = 0,
      .signal_start_id = 0, .signal_end_id = 20,
      .y_min = 0.f, .y_max = 9.f
   }; // local variable to store compute data
   
   const char *io_in = argc > 1 ? argv[1] : "/tmp/sigen.out"; //generator output 
   const char *io_out = argc > 2 ? argv[2] : "/tmp/sigen.in"; //generator input 
   data.fd_in = io_open_read(io_in);
   data.fd_out = io_open_write(io_out);

   if (data.fd_in == -1) {
      fprintf(stderr, "ERROR: Cannot open named pipe %s\n", io_in);
      exit(100);
   }

   if (data.fd_out == -1) {
      fprintf(stderr, "ERROR: Cannot open named pipe %s\n", io_out);
      exit(100);
   }

   enum { KEYBOARD, IO_RX, NUM_THREADS };
   const char *threads_names[] = { "Keyboard", "Pipe In" };

   void* (*thr_functions[])(void*) = { input_keyboard_thread, pipe_io_rx_thread};

   pthread_t threads[NUM_THREADS];
   pthread_mutex_init(&data.mtx, NULL); // initialize mutex with default attributes
   pthread_cond_init(&data.cond, NULL); // initialize mutex with default attributes

   call_termios(0);

   for (int i = 0; i < NUM_THREADS; ++i) {
      int r = pthread_create(&threads[i], NULL, thr_functions[i], &data);
      fprintf(stderr, "INFO: Create thread '%s' %s\n", threads_names[i], ( r == 0 ? "OK" : "FAIL") );
   }

   computation.signal_data = malloc(computation.N * sizeof(float));
   computation.ticks = malloc(computation.N * sizeof(float));
   my_assert(computation.signal_data != NULL, __func__, __LINE__, __FILE__);
   my_assert(computation.ticks != NULL, __func__, __LINE__, __FILE__);
   for (int i = 0; i < computation.N; ++i) {
      computation.ticks[i] = i;
   }
   { // initializace gui and send plot clear to show the window
      xwin_init();
      xwin_set_plot(computation.N, computation.y_min, computation.y_max);
      event ev = { .source = EV_KEYBOARD, .type = EV_CLEAR_PLOT };
      queue_push(ev); 
   }
   bool quit = false;
   while (!quit) {
      event ev = queue_pop();
      switch(ev.source) {
	 case EV_KEYBOARD: // handle keyboard events
	    handle_keyboard(&ev, &computation, &data);
	    break;
	 case EV_PIPE_IN: // handle pipe events
	    handle_pipe_in(&ev, &computation, &data);
	    break;
	 default: // ignore all other events
	    break;
      }
      pthread_mutex_lock(&data.mtx);
      quit = data.quit;
      pthread_mutex_unlock(&data.mtx);
   } // end main quit
   
   fprintf(stderr, "Call queue_cleanup\n");
   queue_cleanup(); // cleanup all events and free allocated memory for messages.
   for (int i = 0; i < NUM_THREADS; ++i) {
      fprintf(stderr, "INFO: Call join to the thread %s\n", threads_names[i]);
      int r = pthread_join(threads[i], NULL);
      fprintf(stderr, "INFO: Joining the thread %s has been %s\n", threads_names[i], (r == 0 ? "OK" : "FAIL"));
   }
   io_close(data.fd_in);
   io_close(data.fd_out);
   xwin_close();
   free(computation.signal_data);
   free(computation.ticks);
   call_termios(1); // restore terminal settings
   return EXIT_SUCCESS;
}

// - function -----------------------------------------------------------------
void handle_keyboard(event *ev, computation_t *computation, data_t *data)
{
   message msg;
   msg.type = MSG_NBR;
   switch(ev->type) {
      case EV_GET_VERSION:
	 { // prepare packet for get version
	    msg.type = MSG_GET_VERSION;
	 }
	 break;
      case EV_SET_SIGNAL: //send set signal only if not computing 
	 if (!computation->computing) {
	    msg.type = MSG_SET_SIGNAL;
	    msg.data.set_signal = data->signal_param;
	    computation->received_data = 0; //reset received_data
	    fprintf(stderr, "DEBUG: Send MSG_SET_SIGNAL\n");
	    // TODO - wait_for_set_signal_ack 
	 }
	 break;
      case EV_RECEIVE_SIGNAL_SINGLE: //request signal
	 if (!computation->computing) { // fill the task
	    fprintf(stderr, "INFO: Set new signal\n");
	    msg.type = MSG_SIGNAL;
	    computation->received_data = 0;
	    computation->computing = true;
	    computation->cid = 0;
	    fprintf(stderr, "INFO: Request new signal\n");
	 } else {
	    fprintf(stderr, "WARN: New computation requested but it is discarded due on ongoing computation\n");
	 }
	 break;
      case EV_RECEIVE_SIGNAL_RANGE:
	 {
	    if (true) {
	       fprintf(stderr, "ERROR: EV_RECEIVE_SIGNAL_RANGE not implemented yet\n");
	    } else {
	       if (!computation->computing) { // fill the task
		  computation->receive_signal_range = true;
		  data->signal_param.heart_rate_mean = HEART_RATE_MEAN_MIN;
		  computation->signal_id = computation->signal_start_id;
		  event ev = { .source = EV_KEYBOARD, .type = EV_SET_SIGNAL };
		  queue_push(ev);
		  fprintf(stderr, "INFO: Request new receive signal range\n");
	       } else {
		  fprintf(stderr, "WARN: New computation requested but it is discarded due on ongoing computation\n");
	       }
	    }
	 }
	 break;
      case EV_CLEAR_PLOT:
	 for (int i = 0; i < computation->N; ++i) {
	    computation->signal_data[i] = 0.0f;
	 }
	 xwin_set_plot(computation->N, computation->y_min, computation->y_max);
	 xwin_redraw_plot(computation->N, computation->ticks, computation->signal_data);
	 break;
      case EV_ABORT: // send abort if computing 
	 if (!computation->computing) {
	    fprintf(stderr, "WARN: Abort requested but it is not computing\n");
	 }
	 msg.type = MSG_ABORT;
	 // TODO - do not forget to clear the computing and receive_signal_range flags
	 computation->computing = false;
	 computation->receive_signal_range = false;
	 break;
      case EV_QUIT: // user initiate quit 
	 if (computation->computing) {
	    msg.type = MSG_ABORT; // send abort if computing
	 }
	 pthread_mutex_lock(&data->mtx);
	 data->quit = true;
	 pthread_cond_broadcast(&data->cond);
	 pthread_mutex_unlock(&data->mtx);
	 break;
      default: // ignore all other messages
	 break;
   }
   if (msg.type != MSG_NBR) { // messge has been set
      if (!send_message(data, &msg)) {
	 fprintf(stderr, "ERROR: send_message() does not send all bytes of the message!\n");
      }
   }
}

// - function -----------------------------------------------------------------
void handle_pipe_in(event *ev, computation_t *computation, data_t *data)
{
   message msg_send = { .type = MSG_NBR };
   message *msg = ev->data.msg;
   switch (msg->type) {
      case MSG_STARTUP:
	 {
	    char str[STARTUP_MSG_LEN+1];
	    for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
	       str[i] = msg->data.startup.message[i];
	    }
	    str[STARTUP_MSG_LEN] = '\0';
	    fprintf(stderr, "INFO: sigen restarted - '%s'\n", str);
	 }
	 break;
      case MSG_OK:
	 fprintf(stderr, "INFO: Receive ok from module\n");
	 // TODO - if wait_for_set_signal_ack and receive_signal_range and not all data in the range received, initiated EV_RECEIVE_SIGNAL_SINGLE 
	 break;
      case MSG_VERSION:
	 if (msg->data.version.patch > 0) {
	    fprintf(stderr, "INFO: sigen ver. %d.%d-p%d\n", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
	 } else {
	    fprintf(stderr, "INFO: sigen ver. %d.%d\n", msg->data.version.major, msg->data.version.minor);
	 }
	 break;
      case MSG_ERROR:
	 if (computation->receive_signal_range && computation->wait_for_set_signal_ack) {
	    computation->wait_for_set_signal_ack = false;
	 }
	 fprintf(stderr, "WARN: Receive error from sigen\n");
	 break;
      case MSG_ABORT:
	 fprintf(stderr, "INFO: Abort from sigen\n");
	 // TODO - do not forget to clear the computing and receive_signal_range flags
	 computation->computing = computation->receive_signal_range = false;
	 break;
      case MSG_SIGNAL_DATA:
	 if (computation->computing) {
	    fprintf(stderr, "DEBUG: Received signal data cid: %d len: %d\n", msg->data.signal_data.cid, msg->data.signal_data.n);
	    if (computation->cid == msg->data.signal_data.cid) {
	       int i = 0;
	       while (
		     computation->received_data < computation->N && 
		     i < msg->data.signal_data.n
		     )
	       {
		  computation->signal_data[computation->received_data++] = msg->data.signal_data.values[i++];
	       }
	       if (i != msg->data.signal_data.n) {
		  fprintf(stderr, "ERROR: Received data cannot fit to the signal_data\n");
		  computation->computing = false;
	       }
	       computation->cid += 1;
	    } else {
	       fprintf(stderr, "WARN: received signal data has cid %d which is different from cid %d - cannot align data to the grid properly\x0a", msg->data.signal_data.cid, computation->cid);
	    }
	 } else {
	    fprintf(stderr, "WARN: sigen sends new data without computing \n");
	 }
	 break;
      case MSG_DONE:
	 if (computation->computing) {
	    computation->computing = false;
	    if (computation->received_data == computation->N) {
	       computation->y_min = FLT_MAX;
	       computation->y_max = -FLT_MAX;
	       // TODO - determine y_min and y_max 
	       // TODO - eventually increase the plot by computation->y_min -= 0.5;
	       // TODO - eventually increase the plot by computation->y_max += 0.5;

	       xwin_set_plot(computation->N, computation->y_min, computation->y_max);
	       xwin_redraw_plot(computation->N, computation->ticks, computation->signal_data);

	       if (computation->receive_signal_range) { //invoke new signal
		  fprintf(stderr, "WARN: Received signal range not implemented yet\n");
	       }
	    } else {
	       fprintf(stderr, "WARN: sigen reports the computation is done but not all expected values have been received %d / %d \n", computation->received_data, computation->N);
	    }
	 } else {
	    fprintf(stderr, "WARN: sigen reports the computation is done but the computing is false\n");
	 }
	 break;
      default:
	 break;
   } // end switch(msg->type)
   if (msg) {
      free(msg);
   }
}

/* end of prga-hw9.c */
