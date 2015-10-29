#include "server.h"


void glut_process_render(void)
{
    process_commands();
    render();
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
  init_display();
  init_socket();
  init_sprite_engine();

  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(640, 480);

  glutCreateWindow("GLUT Program");
  glutDisplayFunc(glut_process_render);
  // glutReshapeFunc(reshape);
  glClearColor(0.3,0.3,0.3,1.0);
  glViewport(0, 0, 640, 480);
  glutMainLoop();
}
