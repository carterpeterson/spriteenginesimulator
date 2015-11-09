#ifndef SESIMULATOR_SESERVER_INPUT_H
#define SESIMULATOR_SESERVER_INPUT_H

/* X library headers */
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

/* Normal C Headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// project specific
#include "render.h"
#include "socket.h"

// button definitions
#define CONTROLLER_0_UP    'w'
#define CONTROLLER_0_DOWN  's'
#define CONTROLLER_0_LEFT  'a'
#define CONTROLLER_0_RIGHT 'd'
#define CONTROLLER_0_A     'j'
#define CONTROLLER_0_B     'i'
#define CONTROLLER_0_X     'h'
#define CONTROLLER_0_Y     'u'

#define CONTROLLER_PLUGGED_IN 0x0100
#define BUTTON_A 0x0080
#define BUTTON_B 0x0040
#define BUTTON_X 0x0020
#define BUTTON_Y 0x0010
#define BUTTON_DOWN  0x0008
#define BUTTON_UP    0x0004
#define BUTTON_LEFT  0x0002
#define BUTTON_RIGHT 0x0001

void init_input(void);

#endif
