// renderer.c

#define OLIVEC_IMPLEMENTATION
#include "external/olive.c"

#define TEXT_BUFFER_SIZE 512
#define COLOR_RGB(R, G, B) OLIVEC_RGBA(R, G, B, 255)

struct {
  u32* pixels;
  u32 width;
  u32 height;
  Olivec_Canvas oc;
  char text_buffer[TEXT_BUFFER_SIZE];
} renderer;

typedef struct Sprite {
  u32* pixels;
  u32 width;
  u32 height;
} Sprite;

static const Sprite sprites[MAX_SPRITE] = {
  [SPRITE_NODE_NONE]  = { .pixels = node_none_pixels, .width = node_none_width, .height = node_none_height, },
  [SPRITE_NODE_CLOCK] = { .pixels = node_clock_pixels, .width = node_clock_width, .height = node_clock_height, },
  [SPRITE_NODE_ADD]   = { .pixels = node_add_pixels, .width = node_add_width, .height = node_add_height, },
  [SPRITE_NODE_IO]    = { .pixels = node_io_pixels, .width = node_io_width, .height = node_io_height, },
  [SPRITE_NODE_AND]   = { .pixels = node_and_pixels, .width = node_and_width, .height = node_and_height, },
  [SPRITE_NODE_PRINT] = { .pixels = node_print_pixels, .width = node_print_width, .height = node_print_height, },
  [SPRITE_NODE_INCR]  = { .pixels = node_incr_pixels, .width = node_incr_width, .height = node_incr_height, },
  [SPRITE_NODE_NOT]   = { .pixels = node_not_pixels, .width = node_not_width, .height = node_not_height, },
};

const u32 color_white = OLIVEC_RGBA(0xff, 0xff, 0xff, 0xff);
const u32 color_black = OLIVEC_RGBA(0x00, 0x00, 0x00, 0xff);
const u32 color_green = OLIVEC_RGBA(0x88, 0xff, 0x88, 0xff);

u32 colors[MAX_COLOR] = {
  [COLOR_WHITE]      = COLOR_RGB(0xff, 0xff, 0xff),
  [COLOR_BLACK]      = COLOR_RGB(0x00, 0x00, 0x00),
  [COLOR_RED]        = COLOR_RGB(0xea, 0x55, 0x55),
  [COLOR_GREEN]      = COLOR_RGB(0x55, 0xf0, 0x55),
  [COLOR_BLUE]       = COLOR_RGB(0x55, 0x55, 0xf0),
  [COLOR_GRAY]       = COLOR_RGB(0x75, 0x75, 0x75),
  [COLOR_PURPLE]     = COLOR_RGB(0xea, 0x95, 0xea),
  [COLOR_CYAN]       = COLOR_RGB(0x55, 0xea, 0xea),
  [COLOR_YELLOW]     = COLOR_RGB(0xdc, 0xb7, 0x23),
  [COLOR_DARK_GREEN] = COLOR_RGB(0x23, 0xaa, 0x23),
  [COLOR_TURQUOSE]   = COLOR_RGB(0x36, 0x77, 0x63),
};

Result renderer_init(u32 width, u32 height) {
  renderer.pixels = calloc(1, sizeof(u32) * width * height);
  assert(renderer.pixels && "renderer_init: failed to allocate memory for pixels");
  renderer.width = width;
  renderer.height = height;
  memset(renderer.text_buffer, 0, sizeof(renderer.text_buffer));
  return Ok;
}

void renderer_begin_frame(u32 background_color) {
  renderer.oc = olivec_canvas(
    renderer.pixels,
    renderer.width,
    renderer.height,
    renderer.width
  );
  olivec_fill(renderer.oc, background_color);
}

void render_rect(i32 x, i32 y, i32 w, i32 h, u32 thickness, u32 color) {
  olivec_frame(renderer.oc, x, y, w, h, thickness, color);
}

void render_fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
  olivec_rect(renderer.oc, x, y, w, h, color);
}

void render_text(i32 x, i32 y, u32 glyph_size, u32 color, char* text) {
  olivec_text(renderer.oc, text, x, y, olivec_default_font, glyph_size, color);
}

void render_sprite(i32 x, i32 y, i32 w, i32 h, u32* pixels, u32 width, u32 height) {
  Olivec_Canvas sprite = olivec_canvas(
    pixels,
    width,
    height,
    width
  );
  olivec_sprite_copy(renderer.oc, x, y, w, h, sprite);
}

void render_sprite_from_id(i32 x, i32 y, i32 w, i32 h, Sprite_id id) {
  if (id >= 0 && id < MAX_SPRITE) {
    const Sprite* sprite = &sprites[id];
    if (sprite->pixels != NULL) {
      render_sprite(x, y, w, h, sprite->pixels, sprite->width, sprite->height);
    }
  }
}

void render_text_format(i32 x, i32 y, u32 glyph_size, u32 color, char* format, ...) {
  va_list argp;
  va_start(argp, format);
  vsnprintf(renderer.text_buffer, TEXT_BUFFER_SIZE, format, argp);
  va_end(argp);
  render_text(x, y, glyph_size, color, renderer.text_buffer);
  renderer.text_buffer[0] = 0;
}

inline u32* renderer_get_pixels() {
  return renderer.oc.pixels;
}

inline u32 color_rgb(u8 r, u8 g, u8 b) {
  return OLIVEC_RGBA(r, g, b, 255);
}

inline u32 color_lerp(u32 from, u32 to, f32 factor) {
  return color_rgb(
    (u8)lerp(OLIVEC_RED(from),   OLIVEC_RED(to),   factor),
    (u8)lerp(OLIVEC_GREEN(from), OLIVEC_GREEN(to), factor),
    (u8)lerp(OLIVEC_BLUE(from),  OLIVEC_BLUE(to),  factor)
  );
}

void renderer_destroy() {
  free(renderer.pixels);
  memset(&renderer, 0, sizeof(renderer));
}
