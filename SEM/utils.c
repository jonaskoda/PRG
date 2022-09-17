
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>               // for STDIN_FILENO

#include "utils.h"

#define MALLOC_ERROR 100
#define ASSERT_ERROR -1

void my_assert(int r, const char *fcname, int line, const char *fname)
{
   if (!(r)) {
      fprintf(stderr, "ERROR: my_assert FAIL: %s() line %d in %s\n", fcname, line, fname);
      exit(ASSERT_ERROR);
   }
}


void *my_malloc(size_t size)
{
    void *ret = malloc(size);
    if (ret == NULL)
    {
        fprintf(stderr, "Error malloc!\n");
        exit (MALLOC_ERROR);
    }
    return ret;
}


void call_termios(int reset)  // serial non block
{
   static struct termios tio, tioOld;
   tcgetattr(STDIN_FILENO, &tio);
   if (reset) {
      tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
   } else {
      tioOld = tio; //backup 
      cfmakeraw(&tio);
      tio.c_oflag |= OPOST;
      tcsetattr(STDIN_FILENO, TCSANOW, &tio);
   }
}
                                // functions for printing specified messages
void info(const char *str) 
{
    fprintf(stderr, "INFO: %s\n", str);
} 

void debug(const char *str)
{
    fprintf(stderr, "DEBUG: %s\n", str);
}

void error(const char *str)
{
    fprintf(stderr, "ERROR: %s\n", str);
}

void warn(const char *str)
{
    fprintf(stderr, "WARN: %s\n", str);
}