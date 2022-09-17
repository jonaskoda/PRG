/*
 * Filename: messages.h
 * Date:     2022/02/05 22:03
 * Author:   Jan Faigl
 */

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <stdint.h>
#include <stdbool.h>

#define STARTUP_MSG_LEN 9
#define SIGNAL_DATA_SIZE 128 // number of values in the data message 

// Definition of the communication messages
typedef enum {
   MSG_OK,               // ack of the received message
   MSG_ERROR,            // report error on the previously received command
   MSG_ABORT,            // abort - from user button or from serial port
   MSG_DONE,             // report the requested work has been done
   MSG_GET_VERSION,      // request version of the firmware
   MSG_VERSION,          // send version of the firmware as major,minor, patch level, e.g., 1.0p1
   MSG_STARTUP,          // init message (id, up to 9 bytes long string, cksum)
   MSG_SET_SIGNAL,       // set signal parameters TODO MSG_SET_COMPUTE
   MSG_SIGNAL,          // request signal of a batch of tasks (chunk_id, ) TODO MSG_COMPUTE
   MSG_SIGNAL_DATA,     // computed result (chunk_id, result) #TODO MSG_COMPUTE_DATA
   MSG_NBR
} message_type;


typedef struct {
   uint8_t major;
   uint8_t minor;
   uint8_t patch;
} msg_version;

typedef struct {
   uint8_t message[STARTUP_MSG_LEN];
} msg_startup;

#define HEART_RATE_MEAN_MIN 10.0
#define HEART_RATE_MEAN_MAX 200.0
#define LOW_FREQ_MIN 0.1
#define LOW_FREQ_MAX 0.5
#define HIGH_FREQ_MIN 0.25
#define HIGH_FREQ_MAX 1.0
#define LOW_HIGH_FREQ_RATIO_MIN 0.0
#define LOW_HIGH_FREQ_RATIO_MAX 1.0

typedef struct {
   double heart_rate_mean; // default: 60; 10--200;
   double low_frequency; // default: 0.1; 0.1--0.5;
   double high_frequency; //default: 0.25; 0.25--1.0;
   double low_high_frequency_ratio; //default: 0.5 0-1;
} msg_set_signal;

typedef struct {
   uint8_t cid;  // chunk id
   uint8_t n; // actual no. of valid data values
   float values[SIGNAL_DATA_SIZE];
} msg_signal_data;

typedef struct {
   uint8_t type;   // message type
   uint8_t cksum; // message command
   union {
      msg_version version;
      msg_startup startup;
      msg_set_signal set_signal;
      msg_signal_data signal_data;
   } data;
} message;

// return the size of the message in bytes
bool get_message_size(uint8_t msg_type, int *size);

// fill the given buf by the message msg (marhaling);
bool fill_message_buf(const message *msg, uint8_t *buf, int size, int *len);

// parse the message from buf to msg (unmarshaling)
bool parse_message_buf(const uint8_t *buf, int size, message *msg);

#endif

/* end of messages.h */
