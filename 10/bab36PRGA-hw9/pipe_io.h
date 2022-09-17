/*
 * Filename: pipe_io.h
 * Date:     2022/04/13 12:55
 * Author:   Jan Faigl
 */

#ifndef __PIPE_IO_H__
#define __PIPE_IO_H__

#include "utils.h"
#include "messages.h"

#ifndef IO_READ_TIMEOUT_MS
#define IO_READ_TIMEOUT_MS 500 //timeout for reading from serial port
#endif

bool send_message(data_t *data, message *msg);
void* pipe_io_rx_thread(void*); // serial receive buffer

#endif

/* end of pipe_io.h */
