#include "render.h"

#define WINDOW_TITLE_STRING "SpriteEngine Server"
Pixel frame_buffer[PIXELS_WIDTH * PIXELS_HEIGHT];
pthread_mutex_t frame_buffer_lock;

void display_frame_buffer(void)
{
  // Do GL things
  //  pthread_mutex_lock(&frame_buffer_lock);
  glDrawPixels(PIXELS_WIDTH, PIXELS_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frame_buffer);
  //pthread_mutex_unlock(&frame_buffer_lock);
  glutSwapBuffers();
}

void init_display(int argc, char **argv)
{
  int i = 0;
  for(; i < PIXELS_WIDTH * PIXELS_HEIGHT; i++) {
    frame_buffer[i].red = 0;
    frame_buffer[i].green = 0;
    frame_buffer[i].blue = 0;
  }

  // glut things
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(EMULATOR_WINDOW_WIDTH, EMULATOR_WINDOW_HEIGHT);
  glutCreateWindow(WINDOW_TITLE_STRING);
  glutDisplayFunc(display_frame_buffer);
  glClearColor(0.3,0.3,0.3,1.0);
  glViewport(0, 0, EMULATOR_WINDOW_WIDTH, EMULATOR_WINDOW_HEIGHT);

  // setup the lock
  pthread_mutex_init(&frame_buffer_lock, NULL);
}

void start_display(void)
{
  glutMainLoop();
}

void set_pixel(int i, int j, Pixel p)
{
  //pthread_mutex_lock(&frame_buffer_lock);
  frame_buffer[(((PIXELS_HEIGHT - 1) - j) * PIXELS_WIDTH) + i] = p;
  //pthread_mutex_unlock(&frame_buffer_lock);
}

void render(void)
{
  glutPostRedisplay();
}
