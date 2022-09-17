/*
 * Filename: xwin_otk_plot.c
 * Date:     2022/02/05 22:33
 * Author:   Jan Faigl
 */

#include <stdio.h>
#include <stdbool.h>

#include <signal.h>
#include <unistd.h>

#include <otk_lib.h>
#include <gadget_lib.h>

#include "xwin_otk_plot.h"

#define WIDTH 640
#define HEIGHT 480

// - module variables --------------------------------------------------------
extern struct OtkObjectInstance *OtkOuterWindow; // 
static _Bool initialized = false;
static OtkWidget plot;

// - function ----------------------------------------------------------------
static void my_assert(int r, const char *fcname, int line, const char *fname)
{
   if (!(r)) {
      fprintf(stderr, "ERROR: my_assert FAIL: %s() line %d in %s\n", fcname, line, fname);
      exit(-1);
   }
}

// - function ----------------------------------------------------------------
void xwin_init(void)
{
   my_assert(!initialized, __func__, __LINE__, __FILE__);
   initialized = true;
   OtkInitWindow( WIDTH, HEIGHT, 0, NULL);
   plot = Otk_Plot_Init( OtkOuterWindow, "Value", "Time step", 2, 4, 96, 90, Otk_Yellow, Otk_Black);
}

// - function ----------------------------------------------------------------
void xwin_close()
{
   my_assert(initialized, __func__, __LINE__, __FILE__);
   Otk_RemoveObject(plot);
}

// - function ----------------------------------------------------------------
void xwin_set_plot(int xrange, float ymin, float ymax)
{
   my_assert(initialized, __func__, __LINE__, __FILE__);
   Otk_RemoveObject(plot);
   plot = Otk_Plot_Init( OtkOuterWindow, "Value", "Time step", 2, 4, 96, 90, Otk_Yellow, Otk_Black);
   Otk_StripChart_Init( plot, xrange, ymin, ymax, Otk_White, Otk_DarkGray, Otk_Red );
}

// - function ----------------------------------------------------------------
void xwin_redraw_plot(int n, float *ticks, float *values)
{
   my_assert(initialized, __func__, __LINE__, __FILE__);
   Otk_Plot_Data( plot, n, ticks, values, Otk_Red);
   xwin_redraw();
}

// - function ----------------------------------------------------------------
void xwin_redraw(void)
{
   Otk_ReDraw_Display();
   OtkUpdateCheck();
}

/* end of xwin_otk_plot.c */
