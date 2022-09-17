
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "exam_utils.h"
#include "save_jpeg.h"
#include "xwin_sdl.h"

#define ERROR_FILE 100
#define ERROR_PROCESS_IMG 101
#define RGB 3

bool process_img(FILE *f, unsigned char *img, int img_width, int img_height, bool anim);
bool swap_image(unsigned char *img, int img_width, int img_height, int w, int h, int x1, int y1, int x2, int y2, bool anim);
bool copy_image(unsigned char *img, int img_width, int img_height, int w, int h, int x1, int y1, int x2, int y2, bool anim);

int main(int argc, char *argv[]) 
{
    int ret = EXIT_SUCCESS;                                                             // reading the arguments
    const char *inputimg = argc > 1? argv[1] : NULL;
    const char *cmnds = argc > 2? argv[2] : NULL; 
    const char *outimg = argc > 3? argv[3] : NULL;
    bool anim = argc > 4 && (strcmp(argv[4], "--anim") == 0)? true : false;

    my_assert(inputimg != NULL && cmnds != NULL && outimg != NULL, __func__,__LINE__,__FILE__);
 // _______________________________________________________________
    int img_width, img_height;
    unsigned char *img = xwin_load_image(inputimg ,&img_width, &img_height);            // read image

    my_assert(*img, __func__,__LINE__,__FILE__);
    if (anim)
    {
        xwin_init(img_width, img_height);
    }
 // _______________________________________________________________
    FILE *f = fopen(cmnds, "r");                                                       // process image
    if (!f) 
    {
        fprintf(stderr, "ERROR: cannot open file %s\n", cmnds);
        return ERROR_FILE;
    }
    if (!process_img(f, img, img_width, img_height, anim))
    {
        fprintf(stderr, "ERROR: cannot process image %s\n", inputimg);
        return ERROR_PROCESS_IMG;
    }
    if (f)
        fclose(f);
 // _______________________________________________________________                    // save the picture
    if (!ret && outimg)
    {
        if (strstr(outimg, ".jpeg") || strstr(outimg, ".jpg"))
        {
            save_image_jpeg(img_width, img_height, img, outimg);
        }
        else
        {
            save_image_ppm(img_width, img_height, img, outimg);
        }
    }
    if (anim)
        xwin_close();
    free(img);
    return ret;
}


bool process_img(FILE *f, unsigned char *img, int img_width, int img_height, bool anim)
{
    int ret = true;
    char comm[3];
    int w, h;
    int x1, y1, x2, y2;
        int cnt = 0;

    while (ret && !feof(f))
    {
        int r = fscanf(f, "%2s %d %d %d %d %d %d", comm, &x1, &y1, &w, &h, &x2, &y2);
        if (r == 7 && strcmp(comm, "sw") == 0)
        {
            ret = swap_image(img, img_width, img_height, w, h, x1, y1, x2, y2, anim);   
        }
        else if (r == 7 && strcmp(comm, "cp") == 0)
        {
            ret = copy_image(img, img_width, img_height, w, h, x1, y1, x2, y2, anim);
        }
        else
        {
            fprintf(stderr, "all arguments have not been read or the command is unknown\n");
            ret = false;
            break;
        }
        cnt++;
        if (anim && cnt % 5 == 0)
        {
            xwin_redraw(img_width, img_height, img);
            delay(100);
        }
    }
    return ret;
}


bool swap_image(unsigned char *img, int img_width, int img_height, int w, int h, int x1, int y1, int x2, int y2, bool anim)
{
    int ret = true;
    return ret;
}


bool copy_image(unsigned char *img, int img_width, int img_height, int w, int h, int x1, int y1, int x2, int y2, bool anim)
{
    int ret = true;
    for (int xi = 0; xi < w; xi++)
    {
        for (int yi = 0; yi < h; yi++)
        {
            const int xs = x1 + xi;
            const int ys = y1 + yi;
            const int xd = x2 + xi;
            const int yd = y2 + yi;
            if (xs >= 0 && xs < img_width && ys >=0 && ys < img_height 
                && xd >=0 && xd < img_width && yd >= 0 && yd < img_height)
            {
                for (int i = 0; i < RGB; i++)
                {
                    img[(((ys * img_width + xs) *3) + i)] = img[(((yd * img_width + xd) * 3) + i)];
                }
            }
            else
            {
                ret = false;
                break;
            }
        }
    }
    return ret;
}