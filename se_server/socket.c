#include "socket.h"

pthread_t server_socket_thread;

int open_local_socket(void)
{
  struct sockaddr_un server_name;
  int sock, size;

  /* Create the socket. */
  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // bind socket to 1985
  server_name.sun_family = AF_LOCAL;
  strncpy(server_name.sun_path, SOCKET_FILENAME, sizeof(server_name.sun_path));
  server_name.sun_path[sizeof(server_name.sun_path) - 1] = '\0';

  size = (offsetof (struct sockaddr_un, sun_path) + strlen (server_name.sun_path));
  if (bind (sock, (struct sockaddr *) &server_name, size) < 0) {
    perror ("bind");
    exit (EXIT_FAILURE);
  }

  return sock;
}

void *server_socket_run_loop(void *argument)
{
  struct sockaddr_un client_name;
  int sockd, accepted_sockd, name_size;
  sockd = open_local_socket();

  listen(sockd, BACKLOG_LEN);
  name_size = sizeof(client_name);
  accepted_sockd = accept(sockd, (struct sockaddr *) &client_name, (socklen_t *) &name_size);
  printf("socket connected\n");
  close(sockd);

  if (accepted_sockd < 0)
    perror("ERROR on accept");

  while (accepted_sockd >= 0) {
    union SECommand command;

    while (read(accepted_sockd, &command, sizeof(union SECommand)) > 0) {
      printf("thing received\n");
    }

  }

  return NULL;
}

void init_socket(void)
{
  pthread_create(&server_socket_thread, NULL, server_socket_run_loop, NULL);
}
