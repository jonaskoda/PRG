/*
 * Filename: xwin_otk_plot.h
 * Date:     2022/02/05 22:31
 * Author:   Jan Faigl
 */

#ifndef __XWIN_OTK_PLOT_H__
#define __XWIN_OTK_PLOT_H__

void xwin_init(void);

void xwin_close(); //TODO

void xwin_set_plot(int xrange, float ymin, float ymax);
void xwin_redraw_plot(int n, float *ticks, float *values);
void xwin_redraw(void);

//TODO ??? void xwin_poll_events(void);

#endif

/* end of xwin_otk_plot.h */
