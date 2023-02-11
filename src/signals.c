// signals.c

#include "signals.h"

#include "external/arg_parser.h"
#include "common.c"
#include "platform.c"
#include "renderer.c"
#include "node.c"

#define MAX_TITLE_LENGTH 64
#define PROG_NAME "Signals"
#define BPM 120.0f

static void signals_render_state_info(State* state);

void signals_render_state_info(State* state) {
  const char* paused_str[] = {"playing", "paused"};
  u32 width = 0;
  u32 height = 0;
  platform_window_size(&width, &height);
  const u32 PADDING = DEFAULT_PADDING;
  render_text_format(PADDING, height - (2 * 20), 2, color_white, "status: %s", paused_str[state->paused == true]);
}

void signals_state_init(State* state) {
  state->dt = 0.0f;
  state->timer = 0.0f;
  state->bpm = BPM;
  state->paused = false;
  node_grid_init(state);
}

i32 signals_start(i32 argc, char** argv) {
  i32 result = EXIT_SUCCESS;

  State state;
  signals_state_init(&state);
  signals_state_load(STATE_PATH, &state);

  const f32 DT_MAX = 0.5f;
  char title[MAX_TITLE_LENGTH] = {0};

  if (platform_window_create("", 800, 600) == Ok) {
      u32 prev = platform_get_ticks();
      u32 current = prev;

      while (platform_pollevents() == Ok) {
        if (!state.paused) {
          current = platform_get_ticks();
          state.dt = (current - prev) * 0.001f;
          prev = current;
          if (state.dt > DT_MAX) {
            state.dt = DT_MAX;
          }
          state.timer += state.dt;
        }

        if (key_pressed[KEY_1]) {
          state.bpm -= 10;
        }
        if (key_pressed[KEY_2]) {
          state.bpm += 10;
        }
        if (key_pressed[KEY_R] && key_mod_ctrl) {
          signals_state_init(&state);
          continue;
        }
        if (key_pressed[KEY_S] && key_mod_ctrl) {
          signals_state_store(STATE_PATH, &state);
          continue;
        }
        if (key_pressed[KEY_L] && key_mod_ctrl) {
          signals_state_load(STATE_PATH, &state);
          continue;
        }
        if (key_pressed[KEY_SPACE]) {
          state.paused = !state.paused;
        }

        renderer_begin_frame(color_rgb(0x20, 0x25, 0x34));

        if (!(state.tick % 32)) {
          snprintf(title, MAX_TITLE_LENGTH, "%s | %.4g bpm | %d fps | %.3g delta", PROG_NAME, state.bpm, (u32)(1.0f / state.dt), state.dt);
          platform_set_title(title);
        }
        nodes_update_and_render(&state);
        signals_render_state_info(&state);
        platform_window_render();
        state.tick++;
      }
      platform_destroy();
  }
  return result;
}

void signals_state_store(const char* path, State* state) {
  Buffer buffer;
  buffer.data = (u8*)state;
  buffer.size = sizeof(State);
  if (file_write(path, &buffer) == Ok) {
    log_info("stored state file `%s`\n", path);
  }
}

void signals_state_load(const char* path, State* state) {
  Buffer buffer;
  if (file_read(path, &buffer) == Ok) {
    if (sizeof(State) != buffer.size) {
      log_error("signals_state_load: tried loading a corrupt/incorrect version of state file `%s`\n", path);
      return;
    }
    memcpy(state, buffer.data, buffer.size);
    buffer_free(&buffer);
    log_info("loaded state file `%s`\n", path);
  }
}
