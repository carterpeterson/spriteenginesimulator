#include "render.h"

#define WINDOW_TITLE_STRING "SpriteEngine Server"
Pixel frame_buffers[2][PIXELS_WIDTH * PIXELS_HEIGHT];
Pixel *write_buffer, *read_buffer;
pthread_mutex_t frame_buffer_lock;
bool update_available;

void display_frame_buffer(void)
{
  // Do GL things
  if (!update_available) // track this to prevent owning the frame_buffer_lock
    return;

  pthread_mutex_lock(&frame_buffer_lock);
  glDrawPixels(PIXELS_WIDTH, PIXELS_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, read_buffer);
  pthread_mutex_unlock(&frame_buffer_lock);
  glutSwapBuffers();
}

void init_display(int argc, char **argv)
{
  int i = 0;
  for(; i < PIXELS_WIDTH * PIXELS_HEIGHT; i++) {
    frame_buffers[0][i].red = 0;
    frame_buffers[0][i].green = 0;
    frame_buffers[0][i].blue = 0;

    frame_buffers[1][i].red = 0;
    frame_buffers[1][i].green = 0;
    frame_buffers[1][i].blue = 0;

  }

  // glut things
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(EMULATOR_WINDOW_WIDTH, EMULATOR_WINDOW_HEIGHT);
  glutCreateWindow(WINDOW_TITLE_STRING);
  glutDisplayFunc(display_frame_buffer);
  glutIdleFunc(display_frame_buffer);
  glClearColor(0.3,0.3,0.3,1.0);
  glViewport(0, 0, EMULATOR_WINDOW_WIDTH, EMULATOR_WINDOW_HEIGHT);

  // setup the lock
  pthread_mutex_init(&frame_buffer_lock, NULL);
  write_buffer = frame_buffers[0];
  read_buffer = frame_buffers[1];
  update_available = false;
}

void start_display(void)
{
  glutMainLoop();
}

void set_pixel(int i, int j, Pixel p)
{
  //pthread_mutex_lock(&frame_buffer_lock); this will always be in same thread as render
  write_buffer[(((PIXELS_HEIGHT - 1) - j) * PIXELS_WIDTH) + i] = p;
  //pthread_mutex_unlock(&frame_buffer_lock);
}

void render(void)
{
  Pixel *temp;

  pthread_mutex_lock(&frame_buffer_lock);
  temp = write_buffer;
  write_buffer = read_buffer;
  read_buffer = temp;
  pthread_mutex_unlock(&frame_buffer_lock);

  update_available = true;
}
