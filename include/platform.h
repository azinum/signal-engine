// platform.h

#ifndef _PLATFORM_H
#define _PLATFORM_H

typedef enum {
  KEY_UNDEFINED,
  KEY_SPACE,
  KEY_LEFT_ARROW,
  KEY_UP_ARROW,
  KEY_RIGHT_ARROW,
  KEY_DOWN_ARROW,
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_A,
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q,
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_W,
  KEY_X,
  KEY_Y,
  KEY_Z,

  MAX_KEY_EVENT,
} KeyEvent;

typedef enum {
  MOUSE_BUTTON_LEFT = 0,
  MOUSE_BUTTON_RIGHT,
  MOUSE_BUTTON_MIDDLE,

  MAX_MOUSE_BUTTON,
} MouseEvent;

extern u8 key_down[];
extern u8 key_pressed[];
extern u32 key_mod_ctrl;

extern i32 mouse_x;
extern i32 mouse_y;
extern u8 mouse_down[MAX_MOUSE_BUTTON];
extern u8 mouse_pressed[MAX_MOUSE_BUTTON];
extern i32 mouse_scroll_x;
extern i32 mouse_scroll_y;

Result platform_window_create(char* title, u32 width, u32 height, u32 vsync);

u32 platform_get_ticks();

Result platform_pollevents();

void platform_window_render();

void platform_set_title(char* title);

void platform_window_size(u32* width, u32* height);

void platform_destroy();

#endif // _PLATFORM_H
