#include <sys/time.h>

#include "socket.h"

pthread_t display_command_socket_thread;
pthread_t display_sync_socket_thread;
pthread_mutex_t command_process_lock;
int controller_sockets[NUM_CONTROLLERS];

struct SocketLoopArgs {
  uint16_t socket_port;
  uint16_t packet_size;
  void (*new_connection_delegate)(void);
  void (*packet_received_delegate)(void *);
};

void command_socket_new_connection_delegate(void);
void command_socket_packet_received_delegate(void *command);
void vsync_socket_new_connection_delegate(void);
void vsync_socket_packet_received_delegate(void *command);

int open_socket_server(uint16_t socket_port)
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

  // bind to socket
  socket_name.sin_family = AF_INET;
  socket_name.sin_addr.s_addr = inet_addr("127.0.0.1");
  socket_name.sin_port = htons(socket_port);
  if (bind (sock, (struct sockaddr *) &socket_name, sizeof(socket_name)) < 0) {
    perror ("bind");
    exit (EXIT_FAILURE);
  }

  return sock;
}

int open_socket_client(uint16_t socket_port)
{
  struct sockaddr_in socket_name;
  int sock;

  /* Create the socket. */
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return -1;
  }

  // bind to socket
  socket_name.sin_family = AF_INET;
  socket_name.sin_addr.s_addr = inet_addr("127.0.0.1");
  socket_name.sin_port = htons(socket_port);

  int rv = connect(sock, (struct sockaddr*) &socket_name, sizeof(struct sockaddr_in));
  if (rv != 0) {
    return -1;
  }

  return sock;
}

void *socket_run_loop(void *argument)
{
  struct sockaddr_in client;
  int sockd, accepted_sockd, size;
  struct SocketLoopArgs *socket_args = argument;
  sockd = open_socket_server(socket_args->socket_port);

  listen(sockd, BACKLOG_LEN);
  size = sizeof(client);
  accepted_sockd = accept(sockd, (struct sockaddr *) &client, (socklen_t *) &size);
  if (accepted_sockd < 0)
    perror("ERROR on accept");

  void *packet = malloc(socket_args->packet_size);

  while (accepted_sockd >= 0) {
    printf("client connected on port:%d\n", socket_args->socket_port);
    socket_args->new_connection_delegate();

    while (read(accepted_sockd, packet, socket_args->packet_size) > 0) {
      socket_args->packet_received_delegate(packet);
    }
    printf("client connection closed on port:%d\n", socket_args->socket_port);
    accepted_sockd = accept(sockd, (struct sockaddr *) &client, (socklen_t *) &size);
  }
  close(sockd);

  free(socket_args);
  return NULL;
}

// dummy vsync thread cause no qemu
void *dummy_vsync_refresh_thread(void *argument)
{
  while (1) {
    pthread_mutex_lock(&command_process_lock);
    // render the current state of the Sprite Engine
    output_sprite_engine_frame();
    pthread_mutex_unlock(&command_process_lock);
  }
}

void init_sockets(void)
{
  // clear the controller file descriptors
  int i = 0;
  for (; i < NUM_CONTROLLERS; i++) {
    controller_sockets[i] = -1;
  }

  // setup the command processing lock
  pthread_mutex_init(&command_process_lock, NULL);

  // start the threads up
  struct SocketLoopArgs *command_socket_arguments = malloc(sizeof(struct SocketLoopArgs));
  command_socket_arguments->socket_port = COMMAND_SOCKET_PORT;
  command_socket_arguments->packet_size = sizeof(union SECommand);
  command_socket_arguments->new_connection_delegate = command_socket_new_connection_delegate;
  command_socket_arguments->packet_received_delegate = command_socket_packet_received_delegate;

  struct SocketLoopArgs *vsync_socket_arguments = malloc(sizeof(struct SocketLoopArgs));
  vsync_socket_arguments->socket_port = SYNC_SOCKET_PORT;
  vsync_socket_arguments->packet_size = sizeof(struct SEVsync);
  vsync_socket_arguments->new_connection_delegate = vsync_socket_new_connection_delegate;
  vsync_socket_arguments->packet_received_delegate = vsync_socket_packet_received_delegate;

  pthread_create(&display_command_socket_thread, NULL, socket_run_loop, command_socket_arguments);
  pthread_create(&display_sync_socket_thread, NULL, dummy_vsync_refresh_thread, vsync_socket_arguments);
  //pthread_create(&display_sync_socket_thread, NULL, socket_run_loop, vsync_socket_arguments);
}

void send_controller_update_packet(int controller, int status_register)
{
  if (controller_sockets[controller] < 0) {
    return; // port never connected
  }

  size_t packet_size = sizeof(int);
  ssize_t rv = send(controller_sockets[controller], &status_register, packet_size, 0);
  if (rv != packet_size) {
    printf("send_controller_update_packet failed to send.\n");
  }
}

void open_controller_sockets(void)
{
  int i;

  // wait for a quick second to give the sockets some time to open.
  usleep(500000);

  for (i = 0; i < NUM_CONTROLLERS; i++) {
    controller_sockets[i] = open_socket_client(CONTROLLER_PORT_0 + i);
  }
}

// Command Processing Socket

void command_socket_new_connection_delegate(void)
{
  //reset_sprite_engine();
  open_controller_sockets();
}

void command_socket_packet_received_delegate(void *command)
{
  if (((union SECommand *) command)->type == UPDATE_OAM) {
    pthread_mutex_lock(&command_process_lock);
    //printf("update oam\n");
    process_command((union SECommand *) command);
    pthread_mutex_unlock(&command_process_lock);
  } else {
    process_command((union SECommand *) command);
  }
}

// Vsync Processing Socket

void vsync_socket_new_connection_delegate(void)
{
  // Do nothing
}

void vsync_socket_packet_received_delegate(void *command)
{
  pthread_mutex_lock(&command_process_lock);
  // render the current state of the Sprite Engine
  output_sprite_engine_frame();
  pthread_mutex_unlock(&command_process_lock);
}
