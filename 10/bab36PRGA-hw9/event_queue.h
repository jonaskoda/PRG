/*
 * Filename: event_queue.h
 * Date:     2017/04/15 12:41
 * Author:   Jan Faigl
 */

#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "messages.h"

typedef enum {
   EV_KEYBOARD,
   EV_PIPE_IN,
   EV_NUM
} event_source;

typedef enum {
   EV_GET_VERSION,
   EV_SET_SIGNAL,
   EV_RECEIVE_SIGNAL_SINGLE,
   EV_RECEIVE_SIGNAL_RANGE,
   EV_CLEAR_PLOT,
   EV_ABORT,
   EV_QUIT,
   EV_TYPE_NUM
} event_type;

typedef struct {
   int param;
} event_keyboard;

typedef struct {
   message *msg;
} event_serial;

typedef struct {
   event_source source;
   event_type type;
   union {
      int param;
      message *msg;
   } data;
} event;

void queue_init(void);
void queue_cleanup(void);

event queue_pop(void);

void queue_push(event ev);

#endif

/* end of event_queue.h */
