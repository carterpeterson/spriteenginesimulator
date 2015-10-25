#include "sprite_engine.h"

// engine definitions

// OAM Registers
struct SpriteOAMRegisterMundane {
  bool enable;
  uint8_t palette;
  bool flip_x, flip_y;
  uint16_t x_offset, y_offset;
};

struct SpriteOAMRegisterInstance {
  bool enable;
  uint8_t palette;
  bool flip_x, flip_y;
  uint16_t x_offset, y_offset;
  uint8_t sprite_size;
  uint8_t sprite;
  bool transpose;
};


// VRAM
struct GrandeSprite {
  uint8_t pixels[GRANDE_SIZE * GRANDE_SIZE];
};

struct VrendeSprite {
  uint8_t pixels[VRENDE_SIZE * VRENDE_SIZE];
};

struct VentiSprite {
  uint8_t pixels[VENTI_SIZE * VENTI_SIZE];
};

struct BackgroundSprite {
  uint8_t pixels[BACKGROUND_WIDTH * BACKGROUND_HEIGHT];
};

struct InstanceSpriteBaseChunk {
  uint8_t pixel[INSTANCE_BASE_SIZE * INSTANCE_BASE_SIZE];
};

// CRAM
struct PaletteColor {
  uint8_t red, green, blue;
};

struct ColorPalette {
  struct PaletteColor colors[PALETTE_COLORS];
};

struct SpriteEngineMemory {
  // OAM Registers
  struct SpriteOAMRegisterMundane grande_oam_registers[NUM_GRANDE_SPRITES];
  struct SpriteOAMRegisterMundane vrende_oam_registers[NUM_VRENDE_SPRITES];
  struct SpriteOAMRegisterMundane venti_oam_registers[NUM_VENTI_SPRITES];
  struct SpriteOAMRegisterMundane background_oam_register;
  struct SpriteOAMRegisterInstance instance_oam_registers[NUM_INSTANCE_CHUNKS];
  bool iprctl;

  // VRAM
  struct GrandeSprite grande_sprites[NUM_GRANDE_SPRITES];
  struct VrendeSprite vrende_sprites[NUM_VRENDE_SPRITES];
  struct VentiSprite venti_sprites[NUM_VENTI_SPRITES];
  struct BackgroundSprite background_sprite;
  struct InstanceSpriteBaseChunk instance_chunks[NUM_INSTANCE_CHUNKS];

  // CRAM
  struct ColorPalette color_palettes[NUM_PALETTES];
};

struct SpriteEngineMemory memory;
struct SpriteOAMRegisterMundane *oam_registers;

struct SECommandListNode {
  union SECommand command;
  struct SECommandListNode *next_node;
};

struct SECommandListNode *command_read_list, *command_write_list;
pthread_mutex_t command_queue_mutex;

void reset_sprite_engine(void)
{
  // free all the things and reset the registers

  // reset the memory map
  memset(&memory, 0x00, sizeof(struct SpriteEngineMemory));
  oam_registers = memory.grande_oam_registers;
}

void init_sprite_engine(void)
{
  command_queue_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  command_read_list = NULL;
  command_write_list = NULL;

  reset_sprite_engine();
}

void queue_command(union SECommand *command)
{
  struct SECommandListNode *new_node = malloc(sizeof(struct SECommandListNode));
  memcpy(&(new_node->command), command, sizeof(union SECommand));

  pthread_mutex_lock(&command_queue_mutex);
  new_node->next_node = command_write_list;
  command_write_list = new_node;
  pthread_mutex_unlock(&command_queue_mutex);
}

void update_pixel(uint x, uint y)
{
  // determine what pixel should apply
  struct PaletteColor p;
  uint i, sprite_x, sprite_y, color_index, palette_index, sprite_size;
  bool found = false;

  if (memory.iprctl) {
    // check mundane

    for (i = 0; i < (NUM_MUNDANE_SPRITES - NUM_BACKGROUND_SPRITES); i++) {
      sprite_size = (i < NUM_GRANDE_SPRITES) ? GRANDE_SIZE :
        (i < (NUM_GRANDE_SPRITES + NUM_VRENDE_SPRITES)) ? VRENDE_SIZE : VENTI_SIZE;

      if (oam_registers[i].enable &&
          ((x >= oam_registers[i].x_offset) && (x < oam_registers[i].x_offset + sprite_size)) &&
          ((y >= oam_registers[i].y_offset) && (y < oam_registers[i].y_offset + sprite_size))) {

        sprite_x = oam_registers[i].flip_x ? (sprite_size - (x - oam_registers[i].x_offset))
          : (x - oam_registers[i].x_offset);
        sprite_y = oam_registers[i].flip_y ? (sprite_size - (y - oam_registers[i].y_offset))
          : (y - oam_registers[i].y_offset);
        palette_index = oam_registers[i].palette;

        switch(sprite_size) {
        case(GRANDE_SIZE):
          color_index = memory.grande_sprites[i].pixels[x + y * sprite_size];
          if (color_index > 0) {
            p = memory.color_palettes[palette_index].colors[color_index];
            found = true;
          }
          break;
        case(VRENDE_SIZE):
          color_index = memory.vrende_sprites[(i - NUM_GRANDE_SPRITES)].pixels[x + y * sprite_size];
          if (color_index > 0) {
            p = memory.color_palettes[palette_index].colors[color_index];
            found = true;
          }
          break;
        case(VENTI_SIZE):
          color_index = memory.venti_sprites[(i - (NUM_GRANDE_SPRITES + NUM_VRENDE_SPRITES))].pixels[x + y * sprite_size];
          if (color_index > 0) {
            p = memory.color_palettes[palette_index].colors[color_index];
            found = true;
          }
          break;
        default:
          // do nothing
          break;
        }

      }
    }
    if (found) {
      set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
      return;
    }
    // check instanced
    // use background
  } else {
    // check instanced
    // check mundane
    // use background
  }
}

void generate_frame_buffer(void)
{
  uint x, y;
  for (y = 0; y < SPRITE_ENGINE_HEIGHT; y++) {
    for (x = 0; x < SPRITE_ENGINE_WIDTH; x++) {
      update_pixel(x, y);
    }
  }
}

void process_commands(void)
{
  struct SECommandListNode *temp;
  union SECommand *command;

  // swap buffers
  pthread_mutex_lock(&command_queue_mutex);
  temp = command_read_list;
  command_read_list = command_write_list;
  command_write_list = temp;
  pthread_mutex_unlock(&command_queue_mutex);

  while (command_read_list != NULL) {
    command = &(command_read_list->command);
    switch (command->type) {
    case UPDATE_OAM :
      if (command->update_oam.oam_index < NUM_MUNDANE_SPRITES) {
        // Update mundane sprite
      } else if (command->update_oam.oam_index >= INSTANCE_INDEX_MIN && command->update_oam.oam_index < INSTANCE_INDEX_MAX) {
        // update instace sprite
      }
      // do nothing otherwise
      break;
    case(SET_PRIORITY_CTRL):
      break;
    case(UPDATE_CRAM):
      break;
    case(UPDATE_VRAM):
      break;
    default:
      // Do nothing
      break;
    }

    command_read_list = command_read_list->next_node;
  }

  generate_frame_buffer();
}
