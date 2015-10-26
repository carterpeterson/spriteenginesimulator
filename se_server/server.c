#include "server.h"

int main(void)
{
  init_display();
  init_socket();
  init_sprite_engine();
  for (;;) {
    // wait here plz
    usleep(16666);

    process_commands();
    render();
  }
}
