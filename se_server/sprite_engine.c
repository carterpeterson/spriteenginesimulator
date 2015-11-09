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
  // CPU Saving flags
  bool active_mundane, active_instance;

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

struct ColorMapping {
  uint8_t palette_index;
  uint8_t color_index;
};

struct DisplayColorMapping {
  struct ColorMapping color_mappings[SPRITE_ENGINE_WIDTH * SPRITE_ENGINE_HEIGHT];
};

struct DisplayColorMapping display_color_mapping;
struct DisplayColorMapping instance_color_mapping;
uint8_t instance_vram_read_chunk[SPRITE_ENGINE_WIDTH * SPRITE_ENGINE_HEIGHT];

void reset_sprite_engine(void)
{
  // free all the things and reset the registers

  // reset the memory map
  memset(&memory, 0x00, sizeof(struct SpriteEngineMemory));
  memset(&display_color_mapping, 0x00, sizeof(struct DisplayColorMapping));
  oam_registers = memory.grande_oam_registers;
}

void test_pattern(void)
{
  oam_registers[0].enable = true;
  oam_registers[0].y_offset = 200;
  oam_registers[0].x_offset = 200;
  int i = 0, j = 0;
  for (; i < GRANDE_SIZE; i++) {
    for (j = 0; j < GRANDE_SIZE; j++) {
      memory.grande_sprites[0].pixels[i + j*GRANDE_SIZE] = ((j*2) / GRANDE_SIZE);
    }
  }
  memory.color_palettes[0].colors[1].red = 255;

  /*memory.vrende_oam_registers[0].enable = true;
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

  for (i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++) {
    memory.background_sprite.pixels[i] = 4;
  }
  memory.color_palettes[0].colors[4].green = 255;
  memory.color_palettes[0].colors[4].blue = 255;
  */

  /*for (i = 0; i < (NUM_MUNDANE_SPRITES - NUM_BACKGROUND_SPRITES); i++) {
    oam_registers[i].enable = true;
    }*/
  memory.background_oam_register.enable = true;
}

void init_sprite_engine(void)
{
  reset_sprite_engine();
  test_pattern();
}

void draw_mundane_sprites(void)
{
  int i, sprite_x, sprite_y, color_index, x_offset, y_offset, sprite_size, frame_x, frame_y;
  uint8_t *pixels;
  i = (NUM_MUNDANE_SPRITES - NUM_BACKGROUND_SPRITES) - 1;

  while(i >= 0) {
    if (oam_registers[i].enable == false) {
      switch(i) {
      case 112:
        i = 55;
        break;
      case 120:
        i = 115;
        break;
      case 56:
        i = 120;
        break;
      case 116:
        i = 111;
        break;
      case 121:
        i = 119;
        break;
      default:
        i--;
        break;
      };
      continue;
    }

    sprite_size = (i < NUM_GRANDE_SPRITES) ? GRANDE_SIZE :
      (i < (NUM_GRANDE_SPRITES + NUM_VRENDE_SPRITES)) ? VRENDE_SIZE : VENTI_SIZE;
    x_offset = oam_registers[i].x_offset;
    y_offset = oam_registers[i].y_offset;

    switch(sprite_size) {
        case(GRANDE_SIZE):
          pixels = &(memory.grande_sprites[i].pixels[0]);
          break;
        case(VRENDE_SIZE):
          pixels = &(memory.vrende_sprites[(i - NUM_GRANDE_SPRITES)].pixels[0]);
          break;
        case(VENTI_SIZE):
          pixels = &(memory.venti_sprites[(i - (NUM_GRANDE_SPRITES + NUM_VRENDE_SPRITES))].pixels[0]);
          break;
        default:
          // do nothing
          break;
        }

    for (sprite_x = 0; sprite_x < sprite_size; sprite_x++) {
      frame_x = (oam_registers[i].flip_y ? (sprite_size - sprite_x) : sprite_x) + x_offset;
      if (frame_x >= SPRITE_ENGINE_WIDTH)
        break;

      for (sprite_y = 0; sprite_y < sprite_size; sprite_y++) {
        frame_y = (memory.instance_oam_registers[i].flip_x ? (sprite_size - sprite_y) : sprite_y) + y_offset;
        if(frame_y >= SPRITE_ENGINE_HEIGHT)
          break;

        color_index = pixels[sprite_x + sprite_y * sprite_size];
        if (color_index > 0) {
          display_color_mapping.color_mappings[frame_x + frame_y * BACKGROUND_WIDTH].color_index = color_index;
          display_color_mapping.color_mappings[frame_x + frame_y * BACKGROUND_WIDTH].palette_index =
            oam_registers[i].palette;
        }
      }
    }

    switch(i) {
    case 112:
      i = 55;
      break;
    case 120:
      i = 115;
      break;
    case 56:
      i = 120;
      break;
    case 116:
      i = 111;
      break;
    case 121:
      i = 119;
      break;
    default:
      i--;
      break;
    };
  }
}

void draw_instance_sprites(void)
{
  int i, j, sprite_x, sprite_y;
  int color_index, x_offset, y_offset;
  int sprite_size_x, sprite_size_y, frame_x, frame_y, base_vram_chunk, current_vram_chunk;
  bool transpose;

  // reset the chunk array && color mapping
  for (i = 0; i < SPRITE_ENGINE_WIDTH * SPRITE_ENGINE_HEIGHT; i++) {
    instance_vram_read_chunk[i] = 0xFF;
    instance_color_mapping.color_mappings[i].palette_index = 0;
    instance_color_mapping.color_mappings[i].color_index = 0;
  }

  i = NUM_INSTANCE_SPRITES - 1;
  for (; i >= 0; i--) {
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
    x_offset = memory.instance_oam_registers[i].x_offset;
    y_offset = memory.instance_oam_registers[i].y_offset;
    transpose = memory.instance_oam_registers[i].transpose;
    base_vram_chunk = memory.instance_oam_registers[i].sprite;

    for (sprite_x = 0; sprite_x < sprite_size_x; sprite_x++) {
      for (sprite_y = 0; sprite_y < sprite_size_y; sprite_y++) {
        if (transpose) {
          frame_x = (memory.instance_oam_registers[i].flip_y ? (sprite_size_x - sprite_y) : sprite_y) + x_offset;
          frame_y = (memory.instance_oam_registers[i].flip_x ? (sprite_size_y - sprite_x) : sprite_x) + y_offset;
        } else {
          frame_x = (memory.instance_oam_registers[i].flip_y ? (sprite_size_x - sprite_x) : sprite_x) + x_offset;
          frame_y = (memory.instance_oam_registers[i].flip_x ? (sprite_size_y - sprite_y) : sprite_y) + y_offset;
        }

        if (frame_x >= SPRITE_ENGINE_WIDTH || frame_y >= SPRITE_ENGINE_HEIGHT)
          continue;

        // determine chunk
        current_vram_chunk = base_vram_chunk + (sprite_x / GRANDE_SIZE) + (sprite_y / GRANDE_SIZE) * 4;
        if (current_vram_chunk > instance_vram_read_chunk[frame_x + frame_y * SPRITE_ENGINE_WIDTH])
          continue;

        instance_vram_read_chunk[frame_x + frame_y * SPRITE_ENGINE_WIDTH] = current_vram_chunk;
        instance_color_mapping.color_mappings[frame_x + frame_y * SPRITE_ENGINE_WIDTH].palette_index =
          memory.instance_oam_registers[i].palette;
        instance_color_mapping.color_mappings[frame_x + frame_y * SPRITE_ENGINE_WIDTH].color_index =
          memory.instance_chunks[current_vram_chunk]
          .pixel[(sprite_x % GRANDE_SIZE) + (sprite_y % GRANDE_SIZE) * INSTANCE_BASE_SIZE];
      }
    }
  }

  for (i = 0; i < SPRITE_ENGINE_WIDTH; i++) {
    for (j = 0; j < SPRITE_ENGINE_HEIGHT; j++) {
      color_index = instance_color_mapping.color_mappings[i + j * SPRITE_ENGINE_WIDTH].color_index;
      if (color_index > 0) {
        display_color_mapping.color_mappings[i + j * SPRITE_ENGINE_WIDTH].color_index = color_index;
        display_color_mapping.color_mappings[i + j * SPRITE_ENGINE_WIDTH].palette_index =
          instance_color_mapping.color_mappings[i + j * SPRITE_ENGINE_WIDTH].palette_index;
      }
    }
  }
}

void draw_background(void)
{
  uint x, y, frame_x, frame_y, x_offset, y_offset;
  x_offset = memory.background_oam_register.x_offset;
  y_offset = memory.background_oam_register.y_offset;

  for (x = 0; x < BACKGROUND_WIDTH; x++) {
    for (y=0; y < BACKGROUND_HEIGHT; y++) {
      frame_x = (memory.background_oam_register.flip_y ? (BACKGROUND_WIDTH - x) : x) + x_offset;
      frame_x = frame_x % BACKGROUND_WIDTH;
      frame_y = (memory.background_oam_register.flip_x ? (BACKGROUND_HEIGHT - y) : y) + y_offset;
      frame_y = frame_y % BACKGROUND_HEIGHT;

      display_color_mapping.color_mappings[frame_x + frame_y * BACKGROUND_WIDTH].color_index =
        memory.background_sprite.pixels[x + y * BACKGROUND_WIDTH];
      display_color_mapping.color_mappings[frame_x + frame_y * BACKGROUND_WIDTH].palette_index =
        memory.background_oam_register.palette;
    }
  }
}

void generate_frame_buffer(void)
{
  struct PaletteColor p;
  uint x, y, palette_index, color_index;

  draw_background();
  if(memory.iprctl) {
    draw_mundane_sprites();
    draw_instance_sprites();
  } else {
    draw_instance_sprites();
    draw_mundane_sprites();
  }

  // set all the pixels
  for (x = 0; x < SPRITE_ENGINE_WIDTH; x++) {
    for (y = 0; y < SPRITE_ENGINE_HEIGHT; y++) {
      color_index = display_color_mapping.color_mappings[x + y * BACKGROUND_WIDTH].color_index;
      palette_index = display_color_mapping.color_mappings[x + y * BACKGROUND_WIDTH].palette_index;
      p = memory.color_palettes[palette_index].colors[color_index];
      set_pixel(x, y, (Pixel) {p.red, p.green, p.blue});
    }
  }
}

void output_sprite_engine_frame(void)
{
  generate_frame_buffer();
  render(); // all this does is schedule a render to happen via glut
}

void process_command(union SECommand *command)
{
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
    memory.color_palettes[command->update_cram.cram_index].colors[command->update_cram.palette_index] =
      (struct PaletteColor) {command->update_cram.red, command->update_cram.green, command->update_cram.blue};
    break;
  case(UPDATE_VRAM):
    {
      uint chunk_index = command->update_vram.chunk;
      uint pixel_x = command->update_vram.pixel_x;
      uint pixel_y = command->update_vram.pixel_y;
      if (command->update_vram.chunk < VRAM_CHUNK_GRANDE_BASE) { // INSTANCE CHUNKS
        memory.instance_chunks[chunk_index].pixel[pixel_x + pixel_y * INSTANCE_BASE_SIZE]
          = command->update_vram.p_data;
      } else if (command->update_vram.chunk < VRAM_CHUNK_VRENDE_BASE) { // GRANDE CHUNKS
        chunk_index -= VRAM_CHUNK_GRANDE_BASE;
        memory.grande_sprites[chunk_index].pixels[pixel_x + pixel_y * GRANDE_SIZE]
          = command->update_vram.p_data;
      } else if (command->update_vram.chunk < VRAM_CHUNK_VENTI_BASE) { // VRENDE CHUNKS
        chunk_index -= VRAM_CHUNK_VRENDE_BASE;
        memory.vrende_sprites[chunk_index / 4]
          .pixels[((chunk_index % 4) / 2) * (2 * GRANDE_SIZE * GRANDE_SIZE)
                  + (chunk_index % 2) * GRANDE_SIZE // align to the 0,0
                  + (pixel_y * 2 * GRANDE_SIZE)
                  + pixel_x] = command->update_vram.p_data;
      } else if (command->update_vram.chunk < VRAM_CHUNK_BACKGROUND_BASE) { // VENTI CHUNKS
        chunk_index -= VRAM_CHUNK_VENTI_BASE;
        memory.vrende_sprites[chunk_index / 16]
          .pixels[((chunk_index % 16) / 4) * (4 * GRANDE_SIZE * GRANDE_SIZE)
                  + (chunk_index % 4) * GRANDE_SIZE
                  + (pixel_y * 4 * GRANDE_SIZE)
                  + pixel_x] = command->update_vram.p_data;
      } else if (command->update_vram.chunk < VRAM_CHUNK_BACKGROUND_MAX) { // BACKGROUND CHUNKS
        chunk_index -= VRAM_CHUNK_BACKGROUND_BASE;
        if (chunk_index < 70) {
          memory.background_sprite.pixels[(chunk_index / 10) * BACKGROUND_WIDTH * GRANDE_SIZE
                                          + (chunk_index % 10) * GRANDE_SIZE
                                          + pixel_y * BACKGROUND_WIDTH
                                          + pixel_x] = command->update_vram.p_data;
        } else {
          uint pixel_index = (chunk_index / 10) * BACKGROUND_WIDTH * GRANDE_SIZE
            + (chunk_index % 5) * 2 * GRANDE_SIZE
            + pixel_y * BACKGROUND_WIDTH
            + pixel_x;
          if (pixel_y >= (GRANDE_SIZE / 2)) {
            pixel_index -= (GRANDE_SIZE / 2) * BACKGROUND_WIDTH;
            pixel_index += GRANDE_SIZE;
          }
          memory.background_sprite.pixels[pixel_index] = command->update_vram.p_data;
        }
      } // else do nothing
      break;
    }
  default:
    // Do nothing
    break;
  }
}

/*void process_commands(void)
{
  struct SECommandListNode *temp, *free_node;
  union SECommand *command;

  // swap buffers
  pthread_mutex_lock(&command_queue_mutex);
  temp = command_read_list;
  command_read_list = command_write_list;
  command_write_list = temp;
  tail_node_write = NULL;
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
      memory.color_palettes[command->update_cram.cram_index].colors[command->update_cram.palette_index] =
        (struct PaletteColor) {command->update_cram.red, command->update_cram.green, command->update_cram.blue};
      break;
    case(UPDATE_VRAM):
      {
        uint chunk_index = command->update_vram.chunk;
        uint pixel_x = command->update_vram.pixel_x;
        uint pixel_y = command->update_vram.pixel_y;
        if (command->update_vram.chunk < VRAM_CHUNK_GRANDE_BASE) { // INSTANCE CHUNKS
          memory.instance_chunks[chunk_index].pixel[pixel_x + pixel_y * INSTANCE_BASE_SIZE]
            = command->update_vram.p_data;
        } else if (command->update_vram.chunk < VRAM_CHUNK_VRENDE_BASE) { // GRANDE CHUNKS
          chunk_index -= VRAM_CHUNK_GRANDE_BASE;
          memory.grande_sprites[chunk_index].pixels[pixel_x + pixel_y * GRANDE_SIZE]
            = command->update_vram.p_data;
        } else if (command->update_vram.chunk < VRAM_CHUNK_VENTI_BASE) { // VRENDE CHUNKS
          chunk_index -= VRAM_CHUNK_VRENDE_BASE;
          memory.vrende_sprites[chunk_index / 4]
            .pixels[((chunk_index % 4) / 2) * (2 * GRANDE_SIZE * GRANDE_SIZE)
                    + (chunk_index % 2) * GRANDE_SIZE // align to the 0,0
                    + (pixel_y * 2 * GRANDE_SIZE)
                    + pixel_x] = command->update_vram.p_data;
        } else if (command->update_vram.chunk < VRAM_CHUNK_BACKGROUND_BASE) { // VENTI CHUNKS
          chunk_index -= VRAM_CHUNK_VENTI_BASE;
          memory.vrende_sprites[chunk_index / 16]
            .pixels[((chunk_index % 16) / 4) * (4 * GRANDE_SIZE * GRANDE_SIZE)
                    + (chunk_index % 4) * GRANDE_SIZE
                    + (pixel_y * 4 * GRANDE_SIZE)
                    + pixel_x] = command->update_vram.p_data;
        } else if (command->update_vram.chunk < VRAM_CHUNK_BACKGROUND_MAX) { // BACKGROUND CHUNKS
          chunk_index -= VRAM_CHUNK_BACKGROUND_BASE;
          if (chunk_index < 70) {
            memory.background_sprite.pixels[(chunk_index / 10) * BACKGROUND_WIDTH * GRANDE_SIZE
                                            + (chunk_index % 10) * GRANDE_SIZE
                                            + pixel_y * BACKGROUND_WIDTH
                                            + pixel_x] = command->update_vram.p_data;
          } else {
            uint pixel_index = (chunk_index / 10) * BACKGROUND_WIDTH * GRANDE_SIZE
              + (chunk_index % 5) * 2 * GRANDE_SIZE
              + pixel_y * BACKGROUND_WIDTH
              + pixel_x;
            if (pixel_y >= (GRANDE_SIZE / 2)) {
              pixel_index -= (GRANDE_SIZE / 2) * BACKGROUND_WIDTH;
              pixel_index += GRANDE_SIZE;
            }
            memory.background_sprite.pixels[pixel_index] = command->update_vram.p_data;
          }
        } // else do nothing
        break;
      }
    default:
      // Do nothing
      break;
    }

    free_node = command_read_list;
    command_read_list = command_read_list->next_node;
    free(free_node);
  }

  generate_frame_buffer();
}*/
