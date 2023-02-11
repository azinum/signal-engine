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

static void signal_state_init(State* state);
static void signal_engine_init(Engine* state);

void signal_state_init(State* state) {
  state->dt = 0.0f;
  state->timer = 0.0f;
  state->bpm = BPM;
  state->paused = false;
  state->camera_x = 0;
  state->camera_y = 0;
  node_grid_init(state);
}

void signal_engine_init(Engine* e) {
  signal_state_init(&e->state);
}

i32 signal_engine_start(i32 argc, char** argv) {
  i32 result = EXIT_SUCCESS;

  Engine engine;
  signal_engine_init(&engine);
  State* state = &engine.state;

  signal_state_init(state);
  signal_engine_state_load(STATE_PATH, state);

  const f32 DT_MAX = 0.5f;
  char title[MAX_TITLE_LENGTH] = {0};

  if (platform_window_create("", 800, 600) == Ok) {
    u32 prev = platform_get_ticks();
    u32 current = prev;

    while (platform_pollevents() == Ok) {
      if (!state->paused) {
        current = platform_get_ticks();
        state->dt = (current - prev) * 0.001f;
        prev = current;
        if (state->dt > DT_MAX) {
          state->dt = DT_MAX;
        }
        state->timer += state->dt;
      }

      if (key_pressed[KEY_1]) {
        state->bpm -= 10;
      }
      if (key_pressed[KEY_2]) {
        state->bpm += 10;
      }
      if (key_pressed[KEY_Q] && key_mod_ctrl) {
        signal_state_init(state);
        continue;
      }
      if (key_pressed[KEY_S] && key_mod_ctrl) {
        signal_engine_state_store(STATE_PATH, state);
        continue;
      }
      if (key_pressed[KEY_R] && key_mod_ctrl) {
        signal_engine_state_load(STATE_PATH, state);
        continue;
      }
      if (key_pressed[KEY_SPACE]) {
        state->paused = !state->paused;
      }

      if (!key_mod_ctrl) {
        if (key_down[KEY_W]) {
          state->camera_y -= 1;
        }
        if (key_down[KEY_S]) {
          state->camera_y += 1;
        }
        if (key_down[KEY_A]) {
          state->camera_x -= 1;
        }
        if (key_down[KEY_D]) {
          state->camera_x += 1;
        }
      }

      renderer_begin_frame(color_rgb(0x20, 0x25, 0x34));

      if (!(state->tick % 32)) {
        snprintf(title, MAX_TITLE_LENGTH, "%s | %.4g bpm | %d fps | %.3g delta", PROG_NAME, state->bpm, (u32)(1.0f / state->dt), state->dt);
        platform_set_title(title);
      }
      nodes_update_and_render(&engine);
      platform_window_render();
      state->tick++;
    }
    platform_destroy();
  }
  return result;
}

void signal_engine_state_store(const char* path, State* state) {
  Buffer buffer;
  buffer.data = (u8*)state;
  buffer.size = sizeof(State);
  if (file_write(path, &buffer) == Ok) {
    log_info("stored state file `%s`\n", path);
  }
}

void signal_engine_state_load(const char* path, State* state) {
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
