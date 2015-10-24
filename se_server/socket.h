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
#define SOCKET_PORT 1985

void init_socket(void);

#endif
