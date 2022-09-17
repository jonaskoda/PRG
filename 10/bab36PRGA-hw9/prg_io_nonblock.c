/*
 * Filename: prg_io_nonblock.c
 * Date:     2019/12/25 14:20
 * Author:   Jan Faigl
 */

#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <poll.h>

#include "prg_io_nonblock.h"
#include "utils.h"

/// ----------------------------------------------------------------------------
static int io_open(const char *fname, int flag)
{
   int fd = open(fname, flag | O_NOCTTY | O_SYNC);
   if (fd == -1) {
      fprintf(stderr, "ERROR: Cannot open file %s\n", fname);
      my_assert(0, __func__, __LINE__, __FILE__);
   }
   // Set fd to non block mode
   int flags = fcntl(fd, F_GETFL);
   flags &= ~O_NONBLOCK;
   my_assert(fcntl(fd, F_SETFL, flags) >= 0, __func__, __LINE__, __FILE__);
   return fd;
}

/// ----------------------------------------------------------------------------
int io_open_read(const char *fname)
{
   // as defined for FIFO, open with be block for read only unless nonblock is specified
   return io_open(fname, O_RDONLY | O_NONBLOCK); 
}

/// ----------------------------------------------------------------------------
int io_open_write(const char *fname)
{
  // return io_open(fname, O_WRONLY | O_NONBLOCK);
   // open if for RDWR even though is is used only for writting, it because it would
   // be otherwise blocked till a thread open the pipr for reading.
   // Since we like to do not block the program, we open it for RDWR
   //
   return io_open(fname, O_RDWR); 
}

/// ----------------------------------------------------------------------------
int io_close(int fd)
{
   return close(fd);
}

/// ----------------------------------------------------------------------------
int io_putc(int fd, char c)
{
   return write(fd, &c, 1);
}

/// ----------------------------------------------------------------------------
int io_getc(int fd)
{
   char c;
   int r = read(fd, &c, 1);
   return r == 1 ? c : -1;
}

/// ----------------------------------------------------------------------------
int io_getc_timeout(int fd, int timeout_ms, unsigned char *c)
{
   struct pollfd ufdr[1];
   int r = 0;
   ufdr[0].fd = fd;
   ufdr[0].events = POLLIN | POLLRDNORM;
   if ((poll(&ufdr[0], 1, timeout_ms) > 0) && (ufdr[0].revents & (POLLIN | POLLRDNORM))) {
      r = read(fd, c, 1);
   }
   return r;
}

/* end of prg_io_nonblock.c */
