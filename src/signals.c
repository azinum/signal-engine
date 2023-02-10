// signals.c

#include "signals.h"

#include "external/arg_parser.h"
#include "common.c"
#include "platform.c"
#include "renderer.c"

#define MAX_TITLE_LENGTH 64
#define PROG_NAME "Signals"

static void state_init(State* state);

i32 signals_start(i32 argc, char** argv) {
  i32 result = EXIT_SUCCESS;

  State state;
  state_init(&state);

  const f32 DT_MAX = 0.5f;
  char title[MAX_TITLE_LENGTH] = {0};

  if (platform_window_create("Signals", 800, 600) == Ok) {
      u32 prev = platform_get_ticks();
      u32 current = prev;

      while (platform_pollevents() == Ok) {
        current = platform_get_ticks();
        state.dt = (current - prev) * 0.001f;
        prev = current;
        if (state.dt > DT_MAX) {
          state.dt = DT_MAX;
        }
        state.total_time += state.dt;

        renderer_begin_frame(color_rgb(0x20, 0x20, 0x25));

        if (!(state.tick % 32)) {
          snprintf(title, MAX_TITLE_LENGTH, "%s | %d fps | %.3g delta", PROG_NAME, (u32)(1.0f / state.dt), state.dt);
          platform_set_title(title);
        }
        platform_window_render();
        state.tick++;
      }
      platform_destroy();
  }
  return result;
}

void state_init(State* state) {
  state->dt = 0.0f;
  state->total_time = 0.0f;
}
