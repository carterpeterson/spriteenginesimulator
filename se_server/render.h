#ifndef SESIMULATOR_SESERVER_RENDER_H
#define SESIMULATOR_SESERVER_RENDER_H

/* X library headers */
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

/* Normal C Headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

// Physical Game Board Related Constants
#define PIXELS_WIDTH            640
#define PIXELS_HEIGHT           480

// Desktop emulation related constants
#define EMULATOR_WINDOW_WIDTH   640
#define EMULATOR_WINDOW_HEIGHT  480

typedef struct {
  uint8_t red, green, blue;
} Pixel;

extern Pixel *frame_buffer;
//extern int width, height, pixel_width, pixel_height;
extern pthread_mutex_t frame_buffer_lock;
extern bool render_flag;

void render(void);
void init_display(void);
void set_pixel(int i, int j, Pixel p);

#endif
