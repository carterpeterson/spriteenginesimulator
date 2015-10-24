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

struct SECommandListNode {
  union SECommand command;
  struct SECommandListNode *next_node;
};

struct SECommandListNode *command_read_list, *command_write_list;
pthread_mutex_t command_queue_mutex;

void init_sprite_engine(void)
{
  command_queue_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  command_read_list = NULL;
  command_write_list = NULL;

  memset(&memory, 0x00, sizeof(struct SpriteEngineMemory));
}

void reset_sprite_engine(void)
{
  // free all the things and reset the registers
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

void generate_frame_buffer(void)
{
  // do nothing for now
}

void process_commands(void)
{
  struct SECommandListNode *temp;

  // swap buffers
  pthread_mutex_lock(&command_queue_mutex);
  temp = command_read_list;
  command_read_list = command_write_list;
  command_write_list = temp;
  pthread_mutex_unlock(&command_queue_mutex);

  generate_frame_buffer();
}
