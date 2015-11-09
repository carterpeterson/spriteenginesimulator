#ifndef SESIMULATOR_SESERVER_SOCKET_H
#define SESIMULATOR_SESERVER_SOCKET_H

/* Normal C Headers */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

// project specific includes
#include "sprite_engine_commands.h"
#include "sprite_engine.h"

// meat and taters
#define BACKLOG_LEN 1
#define COMMAND_SOCKET_PORT 1985
#define SYNC_SOCKET_PORT 1990

// controllers
#define NUM_CONTROLLERS   4
#define CONTROLLER_PORT_0 1986
#define CONTROLLER_PORT_1 1987
#define CONTROLLER_PORT_2 1988
#define CONTROLLER_PORT_3 1989

void init_sockets(void);
void send_controller_update_packet(int controller, int status_register);

#endif
