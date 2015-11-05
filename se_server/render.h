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

void init_display(int argc, char **argv);
void set_pixel(int i, int j, Pixel p);
void render(void);

#endif
