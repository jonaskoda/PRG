/*
 * Filename: pipe_io.c
 * Date:     2022/04/13 12:55
 * Author:   Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>  // for STDIN_FILENO

#include "event_queue.h"
#include "prg_io_nonblock.h"

#include "pipe_io.h"

// - function -----------------------------------------------------------------
bool send_message(data_t *data, message *msg) 
{
   uint8_t msg_buf[sizeof(message)];
   int len;
   fill_message_buf(msg, msg_buf, sizeof(message), &len);
   pthread_mutex_lock(&data->mtx);
   int ret = write(data->fd_out, msg_buf, len);
   pthread_mutex_unlock(&data->mtx);
   //fprintf(stderr, "DEBUG: send_message len: %d ret: %d\n", len, ret);
   return ret == len;
}

// - function -----------------------------------------------------------------
void* pipe_io_rx_thread(void* d) 
{ // read bytes from the serial and puts the parsed message to the queue
   data_t *data = (data_t*)d;
   uint8_t msg_buf[sizeof(message)];
   event ev = { .source = EV_PIPE_IN, .data.msg = NULL };
   int i = 0;
   int len = 0;
   bool end = false;
   unsigned char c;
   while (io_getc_timeout(data->fd_in, IO_READ_TIMEOUT_MS, &c) > 0) {}; // discard garbage

   while (!end) {
      int r = io_getc_timeout(data->fd_in, IO_READ_TIMEOUT_MS, &c);
      if (r > 0) { // character has been read
	 if (i == 0) { // first byte determine the message size
	    if (get_message_size(c, &len)) {
	       // fprintf(stderr, "DEBUG: receive i: %d -- %d - len: %d\n", i, c, len);
	       msg_buf[i++] = c;
	       my_assert(len <= sizeof(message), __func__ , __LINE__, __FILE__); // maximum message size
	    } else {
	       fprintf(stderr, "ERROR: Unknown message type has been received 0x%x\n - '%c'", c, c);
	    }
	 } else { // readings other characters
		  //    fprintf(stderr, "DEBUG: receive i: %d -- %d '%c' \n", i, c, c);
	    msg_buf[i++] = c;
	 }
	 if (len > 0 && i == len) { //complete message has been received
	    message *msg = malloc(sizeof(message));
	    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
	    //    fprintf(stderr, "DEBUG: parse message len: %d\n", len);
	    if (parse_message_buf(msg_buf, len, msg)) {
	       ev.data.msg = msg;
	       queue_push(ev);
	       i = len = 0;
	    } else {
	       fprintf(stderr, "ERROR: Cannot parse message type %d\n", msg_buf[0]);
	       i = len = 0;
	       free(msg);
	    }
	 }
      } else if (r == 0) { //read but nothing has been received
	 if (i > 0) {
	    fprintf(stderr, "WARN: the packet has not been received discard what has been read\n");
	    i = len = 0;
	 }
      } else {
	 fprintf(stderr, "ERROR: Cannot receive data from the serial port\n");
	 end = true;
      }
      pthread_mutex_lock(&data->mtx);
      end = data->quit;
      pthread_mutex_unlock(&data->mtx);
   } // end while
   ev.type = EV_QUIT;
   queue_push(ev);
#ifdef __linux__
   fprintf(stderr, "INFO: Exit io_rx_thread %ld\n", pthread_self());
#else
   fprintf(stderr, "INFO: Exit io_rx_thread %p\n", pthread_self());
#endif
   return NULL;
}



/* end of pipe_io.c */
