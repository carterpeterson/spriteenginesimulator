#include "socket.h"

pthread_t server_socket_thread;

int open_socket(void)
{
  struct sockaddr_in socket_name;
  int sock;

  /* Create the socket. */
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // reuse the address if we messed up closing (likely)
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  // bind socket to 1985
  socket_name.sin_family = AF_INET;
  socket_name.sin_addr.s_addr = inet_addr("127.0.0.1");
  socket_name.sin_port = htons(SOCKET_PORT);

  if (bind (sock, (struct sockaddr *) &socket_name, sizeof(socket_name)) < 0) {
    perror ("bind");
    exit (EXIT_FAILURE);
  }

  return sock;
}

void *server_socket_run_loop(void *argument)
{
  struct sockaddr_in client;
  int sockd, accepted_sockd, size;
  sockd = open_socket();

  listen(sockd, BACKLOG_LEN);
  size = sizeof(client);
  accepted_sockd = accept(sockd, (struct sockaddr *) &client, (socklen_t *) &size);
  printf("socket connected\n");

  if (accepted_sockd < 0)
    perror("ERROR on accept");

  while (accepted_sockd >= 0) {
    union SECommand command;
    reset_sprite_engine();

    while (read(accepted_sockd, &command, sizeof(union SECommand)) > 0) {
      queue_command(&command);
    }
    printf("socket connection closed\n");
    accepted_sockd = accept(sockd, (struct sockaddr *) &client, (socklen_t *) &size);
  }
  close(sockd);

  return NULL;
}

void init_socket(void)
{
  pthread_create(&server_socket_thread, NULL, server_socket_run_loop, NULL);
}
