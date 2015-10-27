#include <sys/time.h>

#include "server.h"

int main(void)
{
  init_display();
  init_socket();
  init_sprite_engine();
  struct timeval start_tv, end_tv;

  for (;;) {
    // wait here plz
    gettimeofday(&start_tv,NULL);
    process_commands();
    render();
    gettimeofday(&end_tv,NULL);

    printf("microseconds for render:%d\n", (end_tv.tv_usec - start_tv.tv_usec));
  }
}
