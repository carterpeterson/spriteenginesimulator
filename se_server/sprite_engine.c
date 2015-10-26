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
  struct SpriteOAMRegisterInstance instance_oam_registers[NUM_INSTANCE_SPRITES];
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

  // test work
  oam_registers = memory.grande_oam_registers;
  oam_registers[0].flip_y = false;
  oam_registers[0].enable = true;
  oam_registers[0].y_offset = 200;
  oam_registers[0].x_offset = 200;
  int i = 0, j = 0;
  for (; i < GRANDE_SIZE; i++) {
    for (j = 0; j < GRANDE_SIZE; j++) {
      memory.grande_sprites[0].pixels[i + j*GRANDE_SIZE] = ((i*2) / GRANDE_SIZE);
    }
  }
  memory.color_palettes[0].colors[1].red = 255;

  memory.vrende_oam_registers[0].enable = true;
  for (; i < VRENDE_SIZE; i++) {
    for (j = 0; j < VRENDE_SIZE; j++) {
      memory.vrende_sprites[0].pixels[i + j*VRENDE_SIZE] = ((i*2) / VRENDE_SIZE) + 1;
    }
  }
  memory.color_palettes[0].colors[2].blue = 255;

  memory.venti_oam_registers[0].enable = true;
  for (i = 0; i < VENTI_SIZE * VENTI_SIZE; i++) {
    memory.venti_sprites[0].pixels[i] = 3;
  }
  memory.color_palettes[0].colors[3].green = 255;

  memory.instance_oam_registers[0].enable = true;
  memory.instance_oam_registers[0].x_offset = 200;
  memory.instance_oam_registers[0].y_offset = 200;
  memory.instance_oam_registers[0].sprite_size = INSTANCE_SIZE_256x128;
  //memory.instance_oam_registers[0].transpose = true;
  memory.instance_oam_registers[0].flip_x = true;
  int chunk = 0;
  for (; chunk < 4; chunk++){
    for (i = 0; i < (INSTANCE_BASE_SIZE * INSTANCE_BASE_SIZE); i++) {
      memory.instance_chunks[chunk].pixel[i] = 5 + (chunk % 2);
    }
  }
  memory.color_palettes[0].colors[5].red = 255;
  memory.color_palettes[0].colors[5].blue = 255;

  memory.color_palettes[0].colors[6].red = 255;
  memory.color_palettes[0].colors[6].green = 255;

  memory.background_oam_register.enable = true;
  for (i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++) {
    memory.background_sprite.pixels[i] = 4;
  }
  memory.color_palettes[0].colors[4].green = 255;
  memory.color_palettes[0].colors[4].blue = 255;
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

int update_pixel_mundane(uint x, uint y)
{
  struct PaletteColor p;
  uint i, sprite_x, sprite_y, color_index, palette_index, sprite_size;
  i = 0;
  while (i < (NUM_MUNDANE_SPRITES - NUM_BACKGROUND_SPRITES)) {
    sprite_size = (i < NUM_GRANDE_SPRITES) ? GRANDE_SIZE :
      (i < (NUM_GRANDE_SPRITES + NUM_VRENDE_SPRITES)) ? VRENDE_SIZE : VENTI_SIZE;

    if (oam_registers[i].enable &&
        ((x >= oam_registers[i].x_offset) && (x < (oam_registers[i].x_offset + sprite_size))) &&
        ((y >= oam_registers[i].y_offset) && (y < (oam_registers[i].y_offset + sprite_size)))) {

      sprite_x = oam_registers[i].flip_y ? (sprite_size - (x - oam_registers[i].x_offset))
        : (x - oam_registers[i].x_offset);
      sprite_y = oam_registers[i].flip_x ? (sprite_size - (y - oam_registers[i].y_offset))
        : (y - oam_registers[i].y_offset);
      palette_index = oam_registers[i].palette;

      switch(sprite_size) {
      case(GRANDE_SIZE):
        color_index = memory.grande_sprites[i].pixels[sprite_x + sprite_y * sprite_size];
        if (color_index > 0) {
          p = memory.color_palettes[palette_index].colors[color_index];
          set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
          return PIXEL_FOUND;
        }
        break;
      case(VRENDE_SIZE):
        color_index = memory.vrende_sprites[(i - NUM_GRANDE_SPRITES)].pixels[sprite_x + sprite_y * sprite_size];
        if (color_index > 0) {
          p = memory.color_palettes[palette_index].colors[color_index];
          set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
          return PIXEL_FOUND;
        }
        break;
      case(VENTI_SIZE):
        color_index = memory.venti_sprites[(i - (NUM_GRANDE_SPRITES + NUM_VRENDE_SPRITES))].pixels[sprite_x + sprite_y * sprite_size];
        if (color_index > 0) {
          p = memory.color_palettes[palette_index].colors[color_index];
          set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
          return PIXEL_FOUND;
        }
        break;
      default:
        // do nothing
        break;
      }
    }
    switch(i) {
    case 55:
      i = 112;
      break;
    case 115:
      i = 120;
      break;
    case 120:
      i = 56;
      break;
    case 111:
      i = 116;
      break;
    case 119:
      i = 121;
      break;
    default:
      i++;
      break;
    };
  }

  return NO_PIXEL_FOUND;
}

int update_pixel_instanced(uint x, uint y)
{
  struct PaletteColor p;
  bool found = false;
  uint i;
  uint sprite_x, sprite_y;
  uint color_index, palette_index, vram_addr = MAX_VRAM_ADDR, possible_vram_addr, oam_index;
  uint sprite_size_x, sprite_size_y;

  for (i = 0; i < NUM_INSTANCE_SPRITES; i++) {
    if (memory.instance_oam_registers[i].enable == false)
      continue;

    switch(memory.instance_oam_registers[i].sprite_size) {
    case (INSTANCE_SIZE_64x64):
      sprite_size_x = 64;
      sprite_size_y = 64;
      break;
    case (INSTANCE_SIZE_128x64):
      sprite_size_x = 128;
      sprite_size_y = 64;
      break;
    case (INSTANCE_SIZE_128x128):
      sprite_size_x = 128;
      sprite_size_y = 128;
      break;
    case (INSTANCE_SIZE_256x128):
      sprite_size_x = 256;
      sprite_size_y = 128;
      break;
    default:
      sprite_size_x = 0;
      sprite_size_y = 0;
    }

    if (memory.instance_oam_registers[i].transpose) {
      uint temp = sprite_size_x;
      sprite_size_x = sprite_size_y;
      sprite_size_y = temp;
    }

    if ((x < memory.instance_oam_registers[i].x_offset) ||
        (x >= (memory.instance_oam_registers[i].x_offset + sprite_size_x)) ||
        (y < memory.instance_oam_registers[i].y_offset) ||
        (y >= (memory.instance_oam_registers[i].y_offset + sprite_size_y)))
      continue;
    sprite_x = x - memory.instance_oam_registers[i].x_offset;
    sprite_y = y - memory.instance_oam_registers[i].y_offset;
    sprite_x = (memory.instance_oam_registers[i].flip_y) ? (sprite_size_x - sprite_x) : sprite_x;
    sprite_y = (memory.instance_oam_registers[i].flip_x) ? (sprite_size_y - sprite_y) : sprite_y;
    if (memory.instance_oam_registers[i].transpose) {
      uint temp = sprite_x;
      sprite_x = sprite_y;
      sprite_y = temp;
    }

    // calculate possible vram addr
    possible_vram_addr = memory.instance_oam_registers[i].sprite * (INSTANCE_BASE_SIZE * INSTANCE_BASE_SIZE);
    possible_vram_addr += (sprite_x / INSTANCE_BASE_SIZE) * (INSTANCE_BASE_SIZE * INSTANCE_BASE_SIZE);
    possible_vram_addr += (sprite_y / INSTANCE_BASE_SIZE) * 4 * (INSTANCE_BASE_SIZE * INSTANCE_BASE_SIZE);
    possible_vram_addr += ((sprite_x % INSTANCE_BASE_SIZE) + (sprite_y % INSTANCE_BASE_SIZE) * INSTANCE_BASE_SIZE);

    if (possible_vram_addr < vram_addr) {
      vram_addr = possible_vram_addr;
      oam_index = i;
      palette_index = memory.instance_oam_registers[i].palette;
      color_index = *(&(memory.instance_chunks[0].pixel[0]) + possible_vram_addr);
      found = true;
    } else if (possible_vram_addr == vram_addr && i < oam_index) {
      oam_index = i;
      palette_index = memory.instance_oam_registers[i].palette;
      color_index = *(&(memory.instance_chunks[0].pixel[0]) + possible_vram_addr);
      found = true;
    }
  }

  if (found && color_index > 0) {
    p = memory.color_palettes[palette_index].colors[color_index];
    set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
    return PIXEL_FOUND;
  }

  return NO_PIXEL_FOUND;
}

void update_pixel_background(uint x, uint y)
{
  struct PaletteColor p;
  uint sprite_x, sprite_y, color_index, palette_index;

  if (memory.background_oam_register.enable == false) {
    set_pixel(x, y, (Pixel) {0,0,0});
    return;
  }

  palette_index = memory.background_oam_register.palette;
  sprite_x = memory.background_oam_register.flip_y ? (BACKGROUND_WIDTH - x) : x;
  sprite_x = (sprite_x + (BACKGROUND_WIDTH - memory.background_oam_register.x_offset)) % BACKGROUND_WIDTH;
  sprite_y = memory.background_oam_register.flip_x ? (BACKGROUND_HEIGHT - y) : y;
  sprite_y = (sprite_y + (BACKGROUND_HEIGHT - memory.background_oam_register.y_offset)) % BACKGROUND_HEIGHT;

  color_index = memory.background_sprite.pixels[sprite_x + sprite_y * BACKGROUND_WIDTH];
  p = memory.color_palettes[palette_index].colors[color_index];
  set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
}

void update_pixel(uint x, uint y)
{
  // determine what pixel should apply
  if (memory.iprctl == false) {
    // check mundane
    if (update_pixel_mundane(x, y) == PIXEL_FOUND)
      return;
    // check instanced
    if (update_pixel_instanced(x, y) == PIXEL_FOUND)
      return;
    // use background
    update_pixel_background(x, y);
  } else {
    if (update_pixel_instanced(x, y) == PIXEL_FOUND)
      return;
    if (update_pixel_mundane(x, y) == PIXEL_FOUND)
      return;
    update_pixel_background(x, y);
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
        oam_registers[command->update_oam.oam_index].enable = command->update_oam.enable;
        oam_registers[command->update_oam.oam_index].palette = command->update_oam.palette;
        oam_registers[command->update_oam.oam_index].flip_x = command->update_oam.flip_x;
        oam_registers[command->update_oam.oam_index].flip_y = command->update_oam.flip_y;
        oam_registers[command->update_oam.oam_index].x_offset = command->update_oam.x_offset;
        oam_registers[command->update_oam.oam_index].y_offset = command->update_oam.y_offset;
      } else if (command->update_oam.oam_index >= INSTANCE_INDEX_MIN && command->update_oam.oam_index < INSTANCE_INDEX_MAX) {
        // update instace sprite
        uint oam_index = command->update_oam.oam_index - INSTANCE_INDEX_MIN;
        memory.instance_oam_registers[oam_index].enable = command->update_oam.enable;
        memory.instance_oam_registers[oam_index].palette = command->update_oam.palette;
        memory.instance_oam_registers[oam_index].flip_x = command->update_oam.flip_x;
        memory.instance_oam_registers[oam_index].flip_y = command->update_oam.flip_y;
        memory.instance_oam_registers[oam_index].x_offset = command->update_oam.x_offset;
        memory.instance_oam_registers[oam_index].y_offset = command->update_oam.y_offset;
        memory.instance_oam_registers[oam_index].sprite_size = command->update_oam.sprite_size;
        memory.instance_oam_registers[oam_index].sprite = command->update_oam.sprite;
        memory.instance_oam_registers[oam_index].transpose = command->update_oam.transpose;
      }
      // do nothing otherwise
      break;
    case(SET_PRIORITY_CTRL):
      memory.iprctl = command->set_priority_control.iprctl;
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
