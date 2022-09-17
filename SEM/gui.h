

#ifndef __GUI_H__
#define __GUI_H__

typedef struct {
    int w;
    int h;
    unsigned char *img;
} gui_structure;

void gui_init(void);
void gui_cleanup(void);

void gui_refresh(bool pc);
void add_to_img(double R, double G, double B, int idx);

#endif