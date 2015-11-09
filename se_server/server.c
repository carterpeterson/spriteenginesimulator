#include "server.h"

int main(int argc, char **argv)
{
  init_display(argc, argv);
  init_sockets();
  init_input();
  init_sprite_engine();
  start_display();
}
