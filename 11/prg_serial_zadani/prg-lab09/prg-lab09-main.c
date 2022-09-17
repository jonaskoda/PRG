
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "prg_serial.h"

void set_raw(_Bool reset);

int main(int argc, char *argv[])
{
   int ret = 0;
   char c;
   const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0";   // specifikovat svoje nucleo
   int fd = serial_open(serial);
   if (fd != -1) { // read from serial port
      set_raw(true); // set the raw mode
      _Bool quit = false;
      while (!quit) {
     if (
           (c = getchar()) == 's' || c == 'e'
        || (c >= '1' && c <= '5')
        ) {
        if (serial_putc(fd, c) == -1) {
           fprintf(stderr, "ERROR: Error in received responses\n");
           quit = true;
        }
     }
     quit = c == 'q';
      } // end while()
      serial_close(fd);
      set_raw(false);
   } else {
      fprintf(stderr, "ERROR: Cannot open device %s\n", serial);
   }
   return ret;
}

void set_raw(_Bool set)
{
   if (set) {
      system("stty raw");  // enable raw
   } else {
      system("stty -raw"); // disable raw
   }
}

// dodelat aby nucleo prijimalo klavesy 1-5 nablikani pro ruzne periody  1 - 0,05
// tenhle proram je hotovy, ale musime upravit porgram na strane nuclea
// pocitaci vlakno z 8 cvika bude jednou za 5s delat prumer z acek, musime tam pridat data
// input thread je potreba upravit aby to cetlo /psalo? hello,1,2,3,4,5,...

void *serialThread(void*);


