/*
 * File name: prg_serial.h
 * Date:      2017/04/07 22:33
 * Author:    Jan Faigl
 */

#ifndef __PRG_SERIAL_H__
#define __PRG_SERIAL_H__

/// ----------------------------------------------------------------------------
/// @brief serial_open
/// 
/// @param fname  -- device name
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_open(const char *fname);

/// ----------------------------------------------------------------------------
/// @brief serial_close
/// 
/// @param fd 
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_close(int fd);

/// ----------------------------------------------------------------------------
/// @brief serial_putc
/// 
/// @param fd 
/// @param c 
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_putc(int fd, char c);

/// ----------------------------------------------------------------------------
/// @brief serial_getc
/// 
/// @param fd 
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_getc(int fd);

int serial_getc_timeout(int fd, int timeout_ms, unsigned char *c);

#endif

/* end of prg_serial.h */
