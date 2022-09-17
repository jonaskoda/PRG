/*
 * Filename: utils.c
 * Date:     2022/02/06 10:23
 * Author:   Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>

#include <termios.h> 
#include <unistd.h>  // for STDIN_FILENO

#include "utils.h"

// - function ----------------------------------------------------------------
void my_assert(int r, const char *fcname, int line, const char *fname)
{
   if (!(r)) {
      fprintf(stderr, "ERROR: my_assert FAIL: %s() line %d in %s\n", fcname, line, fname);
      exit(-1);
   }
}

// - function -----------------------------------------------------------------
void call_termios(int reset)
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

// - function -----------------------------------------------------------------
void limit_value(double *v, double min, double max)
{ 
   if (*v < min) {
      *v = min;
   } 
   if (*v > max) {
      *v = max;
   }
}

// - function -----------------------------------------------------------------
void print_signal_param(const msg_set_signal *param)
{
   fprintf(stderr, "INFO: Signal param heart_rate_mean: %lf low freq.: %lf high freq.: %lf low hifgh freq_ratio: %lf\n",
	 param->heart_rate_mean, param->low_frequency, param->high_frequency, param->low_high_frequency_ratio);
}

/* end of utils.c */
