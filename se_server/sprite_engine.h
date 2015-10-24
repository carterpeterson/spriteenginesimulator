#ifndef SESIMULATOR_SESERVER_SPRITE_ENGINE_H
#define SESIMULATOR_SESERVER_SPRITE_ENGINE_H

/* Normal C Headers */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

// project specific includes
#include "render.h"
#include "sprite_engine_commands.h"


// Sprite Engine Implementation Details
#define PALETTE_COLORS      16 // 15 + Transparent
#define INSTANCE_BASE_SIZE  64
#define NUM_PALETTES        64
#define GRANDE_SIZE         64
#define VRENDE_SIZE         128
#define VENTI_SIZE          256
#define BACKGROUND_WIDTH    640
#define BACKGROUND_HEIGHT   480
#define NUM_GRANDE_SPRITES  112
#define NUM_VRENDE_SPRITES  8
#define NUM_VENTI_SPRITES   2
#define NUM_INSTANCE_CHUNKS 16

void init_sprite_engine(void);
void reset_sprite_engine(void);
void queue_command(union SECommand *command);
void process_commands(void);

#endif
