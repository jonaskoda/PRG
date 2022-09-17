/*
 * Filename: messages.c
 * Date:     2017/04/15 11:22
 * Author:   Jan Faigl
 */

#include <string.h>

#include "messages.h"

// - function  ----------------------------------------------------------------
bool get_message_size(uint8_t msg_type, int *len)
{
   bool ret = true;
   switch(msg_type) {
      case MSG_OK:
      case MSG_ERROR:
      case MSG_ABORT:
      case MSG_DONE:
      case MSG_GET_VERSION:
         *len = 2; // 2 bytes message - id + cksum
         break;
      case MSG_STARTUP:
         *len = 2 + STARTUP_MSG_LEN;
         break;
      case MSG_VERSION:
         *len = 2 + 3 * sizeof(uint8_t); // 2 + major, minor, patch
         break;
      case MSG_SET_SIGNAL:
         *len = 2 + 4 * sizeof(double); // 2 + 4 * params
         break;
      case MSG_SIGNAL:
         *len = 2; // 2
         break;
      case MSG_SIGNAL_DATA:
         *len = 2 + 2 + SIGNAL_DATA_SIZE * sizeof(float); // cid, n, SIGNAL_DATA_SIZE * sizeof(float)
         break;
      default:
         ret = false;
         break;
   }
   return ret;
}

// - function  ----------------------------------------------------------------
bool fill_message_buf(const message *msg, uint8_t *buf, int size, int *len)
{
   if (!msg || size < sizeof(message) || !buf) {
      return false;
   }
   // 1st - serialize the message into a buffer
   bool ret = true;
   *len = 0;
   switch(msg->type) {
      case MSG_OK:
      case MSG_ERROR:
      case MSG_ABORT:
      case MSG_DONE:
      case MSG_GET_VERSION:
         *len = 1;
         break;
      case MSG_STARTUP:
         for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
            buf[i+1] = msg->data.startup.message[i];
         }
         *len = 1 + STARTUP_MSG_LEN;
         break;
      case MSG_VERSION:
         buf[1] = msg->data.version.major;
         buf[2] = msg->data.version.minor;
         buf[3] = msg->data.version.patch;
         *len = 4;
         break;
      case MSG_SET_SIGNAL:
         memcpy(&(buf[1 + 0 * sizeof(double)]), &(msg->data.set_signal.heart_rate_mean), sizeof(double));
         memcpy(&(buf[1 + 1 * sizeof(double)]), &(msg->data.set_signal.low_frequency), sizeof(double));
         memcpy(&(buf[1 + 2 * sizeof(double)]), &(msg->data.set_signal.high_frequency), sizeof(double));
         memcpy(&(buf[1 + 3 * sizeof(double)]), &(msg->data.set_signal.low_high_frequency_ratio), sizeof(double));
         *len = 1 + 4 * sizeof(double);
         break;
      case MSG_SIGNAL:
         *len = 1;
         break;
      case MSG_SIGNAL_DATA:
         buf[1] = msg->data.signal_data.cid;
         buf[2] = msg->data.signal_data.n;
	 memcpy(&(buf[3]), msg->data.signal_data.values, buf[2] * sizeof(float));
         *len = 3 + SIGNAL_DATA_SIZE * sizeof(float);
         break;
      default: // unknown message type
         ret = false;
         break;
   }
   // 2nd - send the message buffer
   if (ret) { // message recognized
      buf[0] = msg->type;
      buf[*len] = 0; // cksum
      for (int i = 0; i < *len; ++i) {
         buf[*len] += buf[i];
      }
      buf[*len] = 255 - buf[*len]; // signal cksum
      *len += 1; // add cksum to buffer
   }
   return ret;
}

// - function  ----------------------------------------------------------------
bool parse_message_buf(const uint8_t *buf, int size, message *msg)
{
   uint8_t cksum = 0;
   for (int i = 0; i < size; ++i) {
      cksum += buf[i];
   }
   bool ret = false;
   int message_size;
   if (
         size > 0 && cksum == 0xff && // sum of all bytes must be 255
         ((msg->type = buf[0]) >= 0) && msg->type < MSG_NBR &&
         get_message_size(msg->type, &message_size) && size == message_size) {
      ret = true;
      switch(msg->type) {
         case MSG_OK:
         case MSG_ERROR:
         case MSG_ABORT:
         case MSG_DONE:
         case MSG_GET_VERSION:
            break;
         case MSG_STARTUP:
            for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
               msg->data.startup.message[i] = buf[i+1];
            }
            break;
         case MSG_VERSION:
            msg->data.version.major = buf[1];
            msg->data.version.minor = buf[2];
            msg->data.version.patch = buf[3];
            break;
         case MSG_SET_SIGNAL: 
            memcpy(&(msg->data.set_signal.heart_rate_mean), &(buf[1 + 0 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.set_signal.low_frequency), &(buf[1 + 1 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.set_signal.high_frequency), &(buf[1 + 2 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.set_signal.low_high_frequency_ratio), &(buf[1 + 3 * sizeof(double)]), sizeof(double));
            break;
         case MSG_SIGNAL: // type + chunk_id + nbr_tasks
            break;
         case MSG_SIGNAL_DATA:  // type + chunk_id + task_id + result
	    msg->data.signal_data.cid = buf[1];
	    msg->data.signal_data.n = buf[2];
	    memcpy(msg->data.signal_data.values, &(buf[3]), buf[2] * sizeof(float));
	    break;
	 default: // unknown message type
	    ret = false;
	    break;
      } // end switch
   }
   return ret;
}

/* end of messages.c */
