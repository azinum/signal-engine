// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

extern const u32 color_white;
extern const u32 color_black;
extern const u32 color_green;

typedef enum {
  COLOR_WHITE,
  COLOR_BLACK,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_GRAY,
  COLOR_PURPLE,
  COLOR_CYAN,

  MAX_COLOR,
} Color;

extern u32 colors[MAX_COLOR];

Result renderer_init(u32 width, u32 height);

void renderer_begin_frame();

void render_rect(i32 x, i32 y, i32 w, i32 h, u32 color);

void render_text(i32 x, i32 y, u32 glyph_size, u32 color, char* text);

void render_text_format(i32 x, i32 y, u32 glyph_size, u32 color, char* format, ...);

u32* renderer_get_pixels();

u32 color_rgb(u8 r, u8 g, u8 b);

u32 color_lerp(u32 from, u32 to, f32 factor);

void renderer_destroy();

#endif // _RENDERER_H
