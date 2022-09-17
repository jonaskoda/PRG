
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "computation.h"
#include "messages.h"
#include "utils.h"
#include "xwin_sdl.h"
#include "gui.h"

#define THREE 3
#define FOUR 4
#define PIXEL_CHANGE 20 
#define DIVIDE_CONSTANT 10
#define IMG_CONSTANT 255

void computation_init(void);
void computation_cleanup(void);
bool is_abort(void);

bool is_computing(void);
bool is_done(void);

void abort_comp(void);

bool set_compute(message *msg); 

bool compute(message *msg);

static struct
{
    double c_re;
    double c_im;
    int n;

    double range_re_min;    // intervals of complex plane
    double range_re_max;
    double range_im_min;
    double range_im_max;

    int grid_w;
    int grid_h;

    int cur_x;
    int cur_y;              // cursors, points somewhere

    double d_re;
    double d_im;

    int nbr_chunks;         // number of chunks
    int cid;                // chunk id 

    double chunk_re;        // starting coords
    double chunk_im;

    uint8_t chunk_n_re;     // number of pixels 
    uint8_t chunk_n_im;

    uint8_t *grid;          // grid for storing iter value for each pixel
    bool abort;
    bool computing;    
    bool done;

} comp = {
    .c_re = -0.4,
    .c_im = 0.6,

    .n = 60,

    .range_re_min = -1.6,
    .range_re_max = 1.6,
    .range_im_min = -1.1,
    .range_im_max = 1.1,
    
    .grid = NULL,
    .grid_w = 640,
    .grid_h = 480,

    .chunk_n_re = 64,     // grid_w / 10
    .chunk_n_im = 48,     // grid_h / 10

    .abort = false,
    .computing = false,
    .done = false
};


void computation_init(void)
{
    comp.grid = my_malloc(comp.grid_w * comp.grid_h);                          // grid for storing iter values
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (1 * comp.grid_w);   // moving from right to left
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (1 * comp.grid_h);  // moving form top to bottom
    comp.chunk_n_re = comp.grid_w / DIVIDE_CONSTANT;
    comp.chunk_n_im = comp.grid_h / DIVIDE_CONSTANT;
    comp.nbr_chunks = (comp.grid_w * comp.grid_h) / (comp.chunk_n_re * comp.chunk_n_im);
}


void computation_cleanup(void)     // free the grid 
{
    if (comp.grid)
    {
        free(comp.grid);
    }
    comp.grid = NULL;
}


bool is_computing(void) {return comp.computing;}
bool is_done(void) {return comp.done;}
bool is_abort(void) {return comp.abort;}

void abort_comp(void) {comp.abort = true, comp.computing = false; }
void enable_comp(void) {comp.abort = false;}


bool set_compute(message *msg)              // set initial values for nucleo
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    bool ret = !is_computing();             // set compute only while not computing
    if (ret)
    {
        msg->type = MSG_SET_COMPUTE;
        msg->data.set_compute.c_re = comp.c_re; 
        msg->data.set_compute.c_im = comp.c_im; 
        msg->data.set_compute.d_re = comp.d_re; 
        msg->data.set_compute.d_im = comp.d_im; 
        msg->data.set_compute.n = comp.n;
        comp.done = false; 
        fprintf(stderr, "INFO: Set new computation resolution %dx%d no. of chunks: %d\n", comp.grid_w, comp.grid_h, comp.nbr_chunks );
    }
    else
    {
        fprintf(stderr, "WARN: New computation requested but it is discarded due on ongoing computation\n\r");
    }
    return ret;
}


bool compute(message *msg)                 // set computation for one chunk -> send info to nucleo
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    if (!is_computing()) // first chunk
    {
        comp.cid = 0;
        comp.computing = true;
        comp.cur_x = comp.cur_y = 0;        // position of the start of the chunk in pixels
        comp.chunk_re = comp.range_re_min;  // upper left corner - starting position in the real numbers
        comp.chunk_im = comp.range_im_max;  // upper left corner of img / starting pos
        msg->type = MSG_COMPUTE;
        fprintf(stderr,"INFO: New computation chunk id: %d for part %d x %d\n", comp.cid, comp.chunk_n_re, comp.chunk_n_im);
    }
    else                 // next chunk
    {
        comp.cid += 1;
        if (comp.cid < comp.nbr_chunks)
        {
            comp.cur_x += comp.chunk_n_re;
            comp.chunk_re += comp.chunk_n_re * comp.d_re;
            if (comp.cur_x >= comp.grid_w)
            {
                comp.cur_x = 0;
                comp.cur_y += comp.chunk_n_im;
                comp.chunk_re = comp.range_re_min;
                comp.chunk_im += (comp.chunk_n_im * comp.d_im);
            }
            msg->type = MSG_COMPUTE;
            fprintf(stderr, "INFO: Prepare new chunk of data cid: %d\n", comp.cid);
        } 
        else             // nothing more to compute
        {
            comp.done = true;
            fprintf(stderr, "INFO: Nucleo reports the computation is done computing: %d\n", comp.computing);
            comp.computing = false;
        }
    }
    if (comp.computing && msg->type == MSG_COMPUTE)
    {
        msg->data.compute.cid = comp.cid;           // index of chunk to be computed
        msg->data.compute.re = comp.chunk_re;
        msg->data.compute.im = comp.chunk_im;       // coords of the starting point 
        msg->data.compute.n_re = comp.chunk_n_re;
        msg->data.compute.n_im = comp.chunk_n_im;   // size of the chunk
    }
    return is_computing(); 
}


void update_image(int w, int h, unsigned char *img) // update picture based on iter values stored in grid array
{
    xwin_poll_events();
    my_assert(img && comp.grid && w == comp.grid_w
                               && h == comp.grid_h,
                      __func__, __LINE__, __FILE__);
    for (int i = 0; i < w * h; ++i)
    {
        const double t = 1.0 * comp.grid[i] / (comp.n + 1.0);  // t = k/n
        *(img++) = 9 * (1 - t) * t * t * t * IMG_CONSTANT;
        *(img++) = 15 * (1 - t) * (1 - t) * t * t * IMG_CONSTANT;
        *(img++) = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * IMG_CONSTANT;
    }
}


void update_data(const msg_compute_data *compute_data)    // take compute data from nucleo and put the iter value 
{                                                         // into the grid array
    my_assert(compute_data != NULL, __func__, __LINE__, __FILE__);
    if (compute_data->cid == comp.cid)
    {
        const int idx = comp.cur_x + compute_data->i_re + (comp.cur_y + compute_data->i_im) * comp.grid_w; // calculate the idx 
        if (idx >= 0 && idx < (comp.grid_w * comp.grid_h))
        {
            comp.grid[idx] = compute_data->iter;
        }
        if ((comp.cid + 1) >= comp.nbr_chunks && (compute_data->i_re +1) == comp.chunk_n_re
            && (compute_data->i_im +1) == comp.chunk_n_im)
            {
                comp.done = true;
                comp.computing = false; 
            }
    }
    else
    {
        fprintf(stderr, "WARN: received compute data has cid %d which is different from cid %d - \
                        cannot align data to the grid properly\x0a", compute_data->cid, comp.cid);
        warn("unexpected chunk id"); 
    }
}


void get_grid_size(int *w, int *h)
{
    *w = comp.grid_w;
    *h = comp.grid_h;
}

void calc_on_pc(message *msg)  // do the whole computation on the pc and update the image at the end
{
    double oldZ_real, oldZ_imag, newZ_imag, newZ_real;

    for (int i = 0; i < comp.grid_h; i++)
    {
        for (int j = 0; j < comp.grid_w; j++)
        {
            newZ_real = comp.range_re_min + (comp.d_re * (double)j);  
            newZ_imag = comp.range_im_max + (comp.d_im * (double)i);
            int iter_cnt = 0;           

            while (iter_cnt < comp.n && (newZ_real * newZ_real + newZ_imag * newZ_imag) < FOUR)  // iter loop
            {
                oldZ_real = newZ_real;
                oldZ_imag = newZ_imag;

                newZ_real = (oldZ_real * oldZ_real) - (oldZ_imag * oldZ_imag) + comp.c_re;
                newZ_imag = (2 * oldZ_real * oldZ_imag) + comp.c_im;
                iter_cnt++;
            }
            const double t = (double)iter_cnt / (double)comp.n;  // t = k/n
            double R = 9 * (1 - t) * t * t * t * IMG_CONSTANT;
            double G = 15 * (1 - t) * (1 - t) * t * t * IMG_CONSTANT;
            double B = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * IMG_CONSTANT;

            int idx = (THREE * (j + (i * comp.grid_w))) - THREE;
            add_to_img(R, G, B, idx);       // add the values into img array
        }
    }
    gui_refresh(true);
}


void reset_chunk_id(void)
{
    comp.cid = 0;
    comp.cur_x = comp.cur_y = 0;        // position of the start of the chunk in pixels
    comp.chunk_re = comp.range_re_min;  // upper left corner - starting position in the real numbers
    comp.chunk_im = comp.range_im_max;  // upper left corner of img / starting pos
}


void clear_buffer(void)
{
    for (int i = 0; i < comp.grid_h * comp.grid_w; i++)
    {
        comp.grid[i] = 0;
    }
}


void refresh_buffer(void)
{
    gui_refresh(false);
}


void load_size(int size_add)           // change the size of the image
{
    comp.grid_w += size_add;
    comp.grid_h += size_add;
}


void change_image_size(void)
{
    gui_cleanup();          // close the image window and free img array
    computation_cleanup();  // free grid array
    reset_chunk_id();
    computation_init();
    gui_init();        
    fprintf(stderr, "new width: %d\n" , comp.grid_w);
    fprintf(stderr, "new height: %d\n" , comp.grid_h);
}


void change_c_constant(double numeric_add)
{
    comp.c_re += numeric_add;
    comp.c_im += numeric_add;
    fprintf(stderr, "new c real value: %f\n" , comp.c_re);
    fprintf(stderr, "new c imaginary value: %f\n" , comp.c_im);

}

