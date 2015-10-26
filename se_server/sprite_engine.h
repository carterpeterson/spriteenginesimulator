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
#define SPRITE_ENGINE_WIDTH  640
#define SPRITE_ENGINE_HEIGHT 480

// Register defines
#define PALETTE_COLORS       16 // 15 + Transparent
#define INSTANCE_BASE_SIZE   64
#define NUM_PALETTES         64
#define GRANDE_SIZE          64
#define VRENDE_SIZE          128
#define VENTI_SIZE           256
#define BACKGROUND_WIDTH     640
#define BACKGROUND_HEIGHT    480
#define NUM_GRANDE_SPRITES   112
#define NUM_VRENDE_SPRITES   8
#define NUM_VENTI_SPRITES    2
#define NUM_BACKGROUND_SPRITES 1
#define NUM_MUNDANE_SPRITES  123
#define NUM_INSTANCE_SPRITES 64
#define NUM_INSTANCE_CHUNKS  16
#define INSTANCE_INDEX_MIN   125
#define INSTANCE_INDEX_MAX   139
#define INSTANCE_SIZE_64x64  0
#define INSTANCE_SIZE_128x64 1
#define INSTANCE_SIZE_128x128 2
#define INSTANCE_SIZE_256x128 3
#define MAX_VRAM_ADDR 0xFFFFFFFF

// reutrns
#define PIXEL_FOUND 0
#define NO_PIXEL_FOUND 1

void init_sprite_engine(void);
void reset_sprite_engine(void);
void queue_command(union SECommand *command);
void process_commands(void);

#endif
