// platform.c

#include <SDL2/SDL.h>

// https://www.freepascal-meets-sdl.net/sdl-2-0-scancode-lookup-table/
const i32 key_map[] = {
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
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
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_0,
  KEY_UNDEFINED, // return
  KEY_UNDEFINED, // escape
  KEY_UNDEFINED, // backspace
  KEY_UNDEFINED, // tab
  KEY_SPACE,

  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,
  KEY_UNDEFINED,

  KEY_RIGHT_ARROW,
  KEY_LEFT_ARROW,
  KEY_DOWN_ARROW,
  KEY_UP_ARROW,
};

static const i32 KEY_MAP_SIZE = (sizeof(key_map) / sizeof(key_map[0]));

u8 key_down[KEY_MAP_SIZE] = {0};
u8 key_pressed[KEY_MAP_SIZE] = {0};
u32 key_mod_ctrl = false;

i32 mouse_x = INT32_MAX;
i32 mouse_y = INT32_MAX;
u8 mouse_down[MAX_MOUSE_BUTTON] = {0};
u8 mouse_pressed[MAX_MOUSE_BUTTON] = {0};
i32 mouse_scroll_x = 0;
i32 mouse_scroll_y = 0;

static SDL_Window* sdl_window = NULL;
static SDL_Renderer* sdl_renderer = NULL;
static SDL_Texture* framebuffer = NULL;

struct {
  u32 width;
  u32 height;
} window = {0};

static Result texture_create(SDL_Renderer* sdl_renderer, SDL_Texture** texture, u32 width, u32 height);

Result texture_create(SDL_Renderer* sdl_renderer, SDL_Texture** texture, u32 width, u32 height) {
  Result result = Ok;

  if (*texture != NULL) {
    SDL_DestroyTexture(*texture);
  }
  *texture = SDL_CreateTexture(
    sdl_renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    width,
    height
  );
  if (!*texture) {
    log_error("failed to create SDL texture: %s\n", SDL_GetError());
    return_defer(Err);
  }
defer:
  return result;
}

Result platform_window_create(char* title, u32 width, u32 height) {
  Result result = Ok;
  window.width = width;
  window.height = height;

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    log_error("failed to initialize SDL: %s\n", SDL_GetError());
    return_defer(Err);
  }
  sdl_window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    0
  );
  if (!sdl_window) {
    log_error("failed to create SDL window: %s\n", SDL_GetError());
    return_defer(Err);
  }

  sdl_renderer = SDL_CreateRenderer(
    sdl_window,
    -1,
    SDL_RENDERER_ACCELERATED
  );
  if (!sdl_renderer) {
    log_error("failed to create SDL renderer: %s\n", SDL_GetError());
    return_defer(Err);
  }

  if (texture_create(sdl_renderer, &framebuffer, width, height) != Ok) {
    log_error("failed to create framebuffer texture: %s\n", SDL_GetError());
    return_defer(Err);
  }

  if (renderer_init(width, height) != Ok) {
    log_error("failed to initialize renderer\n");
    return_defer(Err);
  }
defer:
  return result;
}

u32 platform_get_ticks() {
  return SDL_GetTicks();
}

Result platform_pollevents() {
  Result result = Ok;
  memset(&key_pressed[0], 0, sizeof(key_pressed));
  memset(&mouse_pressed[0], 0, sizeof(mouse_pressed));
  mouse_scroll_x = 0;
  mouse_scroll_y = 0;

  SDL_Keymod key_mod = SDL_GetModState();
  key_mod_ctrl = (key_mod & KMOD_LCTRL) == KMOD_LCTRL;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        return_defer(Done);
      }
      case SDL_KEYDOWN: {
        i32 code = event.key.keysym.scancode;
        if (code >= 0 && code < KEY_MAP_SIZE) {
          u32 mapped_code = key_map[code];
          key_pressed[mapped_code] = !key_down[mapped_code];
          key_down[mapped_code] = true;
        }
        break;
      }
      case SDL_KEYUP: {
        i32 code = event.key.keysym.scancode;
        if (code >= 0 && code < KEY_MAP_SIZE) {
          u32 mapped_code = key_map[code];
          key_pressed[mapped_code] = false;
          key_down[mapped_code] = false;
        }
        break;
      }
      case SDL_MOUSEMOTION: {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        break;
      }
      case SDL_MOUSEBUTTONDOWN: {
        u8 code = event.button.button;
        switch (code) {
          case SDL_BUTTON_LEFT:
            mouse_pressed[MOUSE_BUTTON_LEFT] = !mouse_down[MOUSE_BUTTON_LEFT];
            mouse_down[MOUSE_BUTTON_LEFT] = true;
            break;
          case SDL_BUTTON_MIDDLE:
            break;
          case SDL_BUTTON_RIGHT:
            break;
          default:
            break;
        }
        break;
      }
      case SDL_MOUSEBUTTONUP: {
        u8 code = event.button.button;
        switch (code) {
          case SDL_BUTTON_LEFT:
            mouse_pressed[MOUSE_BUTTON_LEFT] = mouse_down[MOUSE_BUTTON_LEFT] = false;
            break;
          case SDL_BUTTON_MIDDLE:
            break;
          case SDL_BUTTON_RIGHT:
            break;
          default:
            break;
        }
        break;
      }
      case SDL_MOUSEWHEEL: {
        mouse_scroll_x = event.wheel.x;
        mouse_scroll_y = event.wheel.y;
        break;
      }
      default:
        break;
    }
  }
defer:
  return result;
}

void platform_window_render() {
  SDL_Rect window_rect = { 0, 0, window.width, window.height };
  u32* pixels = renderer_get_pixels();
  if (!pixels) {
    return;
  }
  SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
  SDL_RenderClear(sdl_renderer);
  {
    void* pixels_dst;
    int pitch = -1;
    if (SDL_LockTexture(framebuffer, &window_rect, &pixels_dst, &pitch) < 0) {
      log_error("failed to lock framebuffer texture\n");
      return;
    }
    for (u32 y = 0; y < window.height; ++y) {
      memcpy((char*)pixels_dst + y * pitch, pixels + y * window.width, window.width * sizeof(u32));
    }
    SDL_UnlockTexture(framebuffer);
  }
  SDL_RenderCopy(sdl_renderer, framebuffer, &window_rect, &window_rect);
  SDL_RenderPresent(sdl_renderer);
}

void platform_set_title(char* title) {
  SDL_SetWindowTitle(sdl_window, title);
}

void platform_window_size(u32* width, u32* height) {
  if (width) {
    *width = window.width;
  }
  if (height) {
    *height = window.height;
  }
}

void platform_destroy() {
  SDL_DestroyWindow(sdl_window);
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyTexture(framebuffer);
  renderer_destroy();
}
