
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <termios.h> 
#include <unistd.h>  // for STDIN_FILENO
#include <pthread.h>
#include <math.h>

#include "xwin_sdl.h"

struct {
    int w;
    int h;
} var = { 640, 480 };

struct {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double cx;
    double cy;
} coords = {-0.5, 0.5, -0.5, 0.5, -0.4, 0.6};

void fill_img_default(unsigned char *img);
void CalcJulia(unsigned char *img);

int main(int argc, char *argv[]) {
    xwin_init(var.w, var.h); // init window
    unsigned char *img = malloc(3*var.w*var.h*sizeof(unsigned char));
    fill_img_default(img);
    xwin_redraw(var.w, var.h, img);
    CalcJulia(img);
    xwin_redraw(var.w, var.h, img);
    char c = getchar();
    printf("c: %c", c);
    xwin_close();
    free(img);
}

void CalcJulia(unsigned char *img) {
    int i, j, iter;
    double zx, zy, zx2, zy2;
    double x, y;
    double max_r;
    int pos = 0;

    double cx = coords.cx;
    double cy = coords.cy;
    double xmin = coords.xmin;
    double xmax = coords.xmax;
    double ymin = coords.ymin;
    double ymax = coords.ymax;

    int maxiter = 60;

    if (cx*cx + cy*cy > 4) {
        max_r = cx * cx + cy * cy;
    } else {
        max_r = 4.0;
    }
    y = ymin;

    for (j=0; j<var.h; j++) { // pro vsechny radky v bitmape

        x=xmin;

        for (i=0; i<var.w; i++) { // pro vsechny sloupce v bitmape

            zx=x; zy=y; // nastavit komplexni promennou "z"
            iter=0; // vynulovat pocet iteraci

            do { // iteracni smycka

                zx2=zx*zx; // zx^2
                zy2=zy*zy; // zy^2
                zy=2.0*zx*zy+cy;
                zx=zx2-zy2+cx; // z:=z^2+c
                iter++; // zvysit pocet iteraci

            } while (iter<maxiter && (zx2+zy2)<max_r); // test na poc. iteraci a bailout 

            if (iter==maxiter) { // bod je uvnitr mnoziny
                
                int zero = 0;
                img[pos++] = (unsigned char) zero; // cerny pixel
                img[pos++] = (unsigned char) zero;
                img[pos++] = (unsigned char) zero;

            }

            else { // bod je vne mnoziny

                double t = ((double) iter)/maxiter; // t
                double t_2 = t * t;            // t^2
                double t_3 = t_2 * t;          // t^3

                double num = 255;
                double one_minus_t_1 = (1 - t);                      // (t-1)
                double one_minus_t_2 = one_minus_t_1 * one_minus_t_1; // (t-1)^2
                double one_minus_t_3 = one_minus_t_2 * one_minus_t_1; // (t-1)^3

                int R_int = (int) (9 * one_minus_t_1 * t_3 * num);
                int G_int = (int) (15 * one_minus_t_2 * t_2 * num);  
                int B_int  = (int) (8.5 * one_minus_t_3 * t * num);

                unsigned char R = (unsigned char) R_int;
                unsigned char G = (unsigned char) G_int;
                unsigned char B = (unsigned char) B_int;
                
                img[pos++] = R;
                img[pos++] = G;
                img[pos++] = B;

            }

            x+=(xmax-xmin)/var.w; // dalsi bod v komplexni rovine
            //printf("del1: %f", xmax - xmin);

        }

        y+=(ymax-ymin)/var.h;

    }

}

void fill_img_default(unsigned char *img) {
   int R = 124; // alza barva
   int G = 252;
   int B = 0;
   int pos = 0;

   for (int row = 0; row < var.h; row++) {
      for (int col = 0; col < var.w; col++) {
         img[pos++] = (unsigned char) R;
         img[pos++] = (unsigned char) G;
         img[pos++] = (unsigned char) B;
      }
   }
}
