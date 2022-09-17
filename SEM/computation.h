
#include <stdint.h>
#include <stdbool.h>
#include "messages.h"

#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__

bool is_computing(void);
bool is_done(void);
bool is_abort(void);
 
void computation_init(void);
void computation_cleanup(void);

void get_grid_size(int *w, int *h);

bool is_computing(void);
bool is_done(void);

void abort_comp(void);
void enable_comp(void);

bool set_compute(message *msg);  
bool compute(message *msg);

void update_image(int w, int h, unsigned char *img);

void update_data(const msg_compute_data *compute_data);
void* my_malloc(size_t size);

void calc_on_pc(message *msg);
void reset_chunk_id(void);

void clear_buffer(void);
void refresh_buffer(void);

void load_size(int size_add);
void change_image_size(void);
void change_c_constant(double numeric_add);

#endif