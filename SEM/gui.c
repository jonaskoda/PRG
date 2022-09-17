
#include <stdlib.h>
#include <stdio.h>

#include "xwin_sdl.h"
#include "computation.h"
#include "gui.h"
#include "utils.h"

#define RGB 3

gui_structure gui = { .img = NULL };

void gui_init(void)        // initialize img for storing RGB values and initialize image
{
    get_grid_size(&gui.w, &gui.h);
    gui.img = my_malloc(gui.w * gui.h * RGB);
    xwin_init(gui.w, gui.h);
}


void gui_cleanup(void)
{
    if (gui.img)
    {
        free(gui.img);
        gui.img = NULL;
    }
    xwin_close();
}


void gui_refresh(bool pc)                          // refresh the image
{
    if (gui.img)
    {
        if (!pc)
        {
            update_image(gui.w, gui.h, gui.img);   // update picture based on iter values stored in grid array
            xwin_redraw(gui.w, gui.h, gui.img);    
        } 
        else                                       // case when update_im is not needed
            xwin_redraw(gui.w, gui.h, gui.img);
    }
}

void add_to_img(double R, double G, double B, int idx) // add RGB values into img array
{
    gui.img[idx++] = R;
    gui.img[idx++] = G;
    gui.img[idx] = B;
}

