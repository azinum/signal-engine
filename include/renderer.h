// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

typedef enum {
  COLOR_WHITE,
  COLOR_BLACK,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_GRAY,
  COLOR_PURPLE,
  COLOR_CYAN,
  COLOR_YELLOW,
  COLOR_TURQUOSE,
  COLOR_DARK_GREEN,
  COLOR_DARK_GRAY,

  MAX_COLOR,
} Color;

extern u32 colors[MAX_COLOR];

typedef enum {
  SPRITE_NODE_NONE = 0,
  SPRITE_NODE_CLOCK,
  SPRITE_NODE_ADD,
  SPRITE_NODE_IO,
  SPRITE_NODE_AND,
  SPRITE_NODE_PRINT,
  SPRITE_NODE_INCR,
  SPRITE_NODE_NOT,
  SPRITE_NODE_COPY,
  SPRITE_NODE_EQUALS,
  SPRITE_NODE_COPY_LR,
  SPRITE_NODE_COPY_RL,
  SPRITE_NODE_COPY_UD,
  SPRITE_NODE_COPY_DU,

  MAX_SPRITE,
} Sprite_id;

Result renderer_init(u32 width, u32 height);

void renderer_begin_frame();

void render_rect(i32 x, i32 y, i32 w, i32 h, u32 thickness, u32 color);

void render_fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color);

void render_text(i32 x, i32 y, u32 glyph_size, u32 color, char* text);

void render_sprite_from_id(i32 x, i32 y, i32 w, i32 h, Sprite_id id);

void render_sprite(i32 x, i32 y, i32 w, i32 h, u32* pixels, u32 width, u32 height);

void render_text_format(i32 x, i32 y, u32 glyph_size, u32 color, char* format, ...);

u32* renderer_get_pixels();

u32 color_rgb(u8 r, u8 g, u8 b);

u32 color_lerp(u32 from, u32 to, f32 factor);

void renderer_destroy();

#endif // _RENDERER_H
