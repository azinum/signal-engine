// signal_engine.c

#include "signal_engine.h"

#include "external/arg_parser.h"
#include "common.c"
#include "math_util.c"
#include "platform.c"
#include "renderer.c"
#include "node.c"
#include "camera.c"

#define MAX_TITLE_LENGTH 64
#define PROG_NAME "Signal Engine"
#define BPM 120.0f

i32 saved_mouse_x = 0;
i32 saved_mouse_y = 0;
i32 saved_camera_x = 0;
i32 saved_camera_y = 0;

static void signal_state_init(State* state);
static void signal_engine_init(Engine* state);

void signal_state_init(State* state) {
  state->dt = 0.0f;
  state->timer = 0.0f;
  state->bpm = BPM;
  state->paused = false;
  camera_init(&state->camera);
  node_grid_init(state);
}

void signal_engine_init(Engine* e) {
  signal_state_init(&e->state);
  e->show_info_box = true;
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
      current = platform_get_ticks();
      state->dt = (current - prev) * 0.001f;
      prev = current;
      if (state->dt > DT_MAX) {
        state->dt = DT_MAX;
      }
      if (!state->paused) {
        state->timer += state->dt;
      }

      if (key_mod_ctrl) {
        if (key_pressed[KEY_Q]) {
          signal_state_init(state);
        }
        if (key_pressed[KEY_S]) {
          signal_engine_state_store(STATE_PATH, state);
        }
        if (key_pressed[KEY_R]) {
          signal_engine_state_load(STATE_PATH, state);
        }
      }
      else {
        if (key_pressed[KEY_SPACE]) {
          state->paused = !state->paused;
        }
        if (key_pressed[KEY_M]) {
          engine.show_info_box = !engine.show_info_box;
        }
        if (key_pressed[KEY_1]) {
          state->bpm -= 10;
        }
        if (key_pressed[KEY_2]) {
          state->bpm += 10;
        }

        if (key_down[KEY_W]) {
          state->camera.target_y-= CAMERA_SPEED * state->dt;
        }
        if (key_down[KEY_S]) {
          state->camera.target_y += CAMERA_SPEED * state->dt;
        }
        if (key_down[KEY_A]) {
          state->camera.target_x -= CAMERA_SPEED * state->dt;
        }
        if (key_down[KEY_D]) {
          state->camera.target_x += CAMERA_SPEED * state->dt;
        }
      }

      if (mouse_pressed[MOUSE_BUTTON_MIDDLE]) {
        saved_camera_x = state->camera.target_x;
        saved_camera_y = state->camera.target_y;
        saved_mouse_x = mouse_x;
        saved_mouse_y = mouse_y;
      }
      else if (mouse_down[MOUSE_BUTTON_MIDDLE]) {
        state->camera.target_x = state->camera.x = saved_camera_x - (mouse_x - saved_mouse_x);
        state->camera.target_y = state->camera.y = saved_camera_y - (mouse_y - saved_mouse_y);
      }

      camera_update(&engine, &state->camera);

      renderer_begin_frame(color_rgb(0x20, 0x25, 0x34));

      if (!(state->tick % 16)) {
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
