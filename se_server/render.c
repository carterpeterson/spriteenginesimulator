#include "render.h"

#define WINDOW_TITLE_STRING "SpriteEngine Server"

int width, height, pixel_width, pixel_height;
Pixel pixel_buffers[2][PIXELS_WIDTH * PIXELS_HEIGHT];
Pixel *read_buffer, *frame_buffer;

pthread_mutex_t frame_buffer_lock;
bool render_flag;

void init_display(void)
{
  pthread_mutex_init(&frame_buffer_lock, NULL);
  render_flag = false;

  frame_buffer = pixel_buffers[0];
  read_buffer = pixel_buffers[1];

  int i = 0;
  for(; i < PIXELS_WIDTH * PIXELS_HEIGHT; i++) {
    frame_buffer[i].red = 0;
    frame_buffer[i].green = 0;
    frame_buffer[i].blue = 0;
  }
}

void render_frame_buffer()
{
  // Do GL things
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawPixels(PIXELS_WIDTH, PIXELS_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frame_buffer);
  glutSwapBuffers();
}

void set_pixel(int i, int j, Pixel p)
{
  render_flag = true;
  frame_buffer[(((PIXELS_HEIGHT - 1) - j) * PIXELS_WIDTH) + i] = p;
}

void render()
{
  Pixel *temp;

  if (render_flag == false)
    return;

  pthread_mutex_lock(&frame_buffer_lock);
  temp = read_buffer;
  read_buffer = frame_buffer;
  frame_buffer = read_buffer;

  render_flag = false;
  pthread_mutex_unlock(&frame_buffer_lock);
  render_frame_buffer();
}
