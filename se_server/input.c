#include "input.h"

uint32_t controller_states[NUM_CONTROLLERS];

void key_pressed_callback(unsigned char key, int x, int y)
{
  int controller = -1;
  switch (key) {
  case CONTROLLER_0_UP:
    controller_states[0] |= BUTTON_UP;
    controller = 0;
    break;
  case CONTROLLER_0_DOWN:
    controller_states[0] |= BUTTON_DOWN;
    controller = 0;
    break;
  case CONTROLLER_0_LEFT:
    controller_states[0] |= BUTTON_LEFT;
    controller = 0;
    break;
  case CONTROLLER_0_RIGHT:
    controller_states[0] |= BUTTON_RIGHT;
    controller = 0;
    break;
  case CONTROLLER_0_A:
    controller_states[0] |= BUTTON_A;
    controller = 0;
    break;
  case CONTROLLER_0_B:
    controller_states[0] |= BUTTON_B;
    controller = 0;
    break;
  case CONTROLLER_0_X:
    controller_states[0] |= BUTTON_X;
    controller = 0;
    break;
  case CONTROLLER_0_Y:
    controller_states[0] |= BUTTON_Y;
    controller = 0;
    break;
  default:
    // do nothing
    break;
  }

  if (controller != -1)
    send_controller_update_packet(controller, controller_states[controller]);
}

void key_up_callback(unsigned char key, int x, int y)
{
  int controller = -1;
  switch (key) {
  case CONTROLLER_0_UP:
    controller_states[0] &= ~BUTTON_UP;
    controller = 0;
    break;
  case CONTROLLER_0_DOWN:
    controller_states[0] &= ~BUTTON_DOWN;
    controller = 0;
    break;
  case CONTROLLER_0_LEFT:
    controller_states[0] &= ~BUTTON_LEFT;
    controller = 0;
    break;
  case CONTROLLER_0_RIGHT:
    controller_states[0] &= ~BUTTON_RIGHT;
    controller = 0;
    break;
  case CONTROLLER_0_A:
    controller_states[0] &= ~BUTTON_A;
    controller = 0;
    break;
  case CONTROLLER_0_B:
    controller_states[0] &= ~BUTTON_B;
    controller = 0;
    break;
  case CONTROLLER_0_X:
    controller_states[0] &= ~BUTTON_X;
    controller = 0;
    break;
  case CONTROLLER_0_Y:
    controller_states[0] &= ~BUTTON_Y;
    controller = 0;
    break;
  default:
    // do nothing
    break;
  }

  if (controller != -1)
    send_controller_update_packet(controller, controller_states[controller]);
}


void init_input(void)
{
  glutKeyboardFunc(&key_pressed_callback);
  glutKeyboardUpFunc(&key_up_callback);

  int i = 0;
  for (; i < NUM_CONTROLLERS; i++) {
    controller_states[i] = CONTROLLER_PLUGGED_IN;
  }
}
