// signal_engine.h

#ifndef _SIGNAL_ENGINE_H
#define _SIGNAL_ENGINE_H

#include <time.h>

#include "common.h"
#include "math_util.h"
#include "assets.h"
#include "platform.h"
#include "renderer.h"
#include "node.h"
#include "camera.h"

#define NODE_GRID_WIDTH 64
#define NODE_GRID_HEIGHT 64
#define MAX_NODE (NODE_GRID_WIDTH * NODE_GRID_HEIGHT)

#define DEFAULT_PADDING 2
#define DEFAULT_GLYPH_SIZE 2

#define LOG_ENTRY_LENGTH 64
#define MAX_LOG_ENTRY 16

#define TIMER_START(...) \
  struct timespec _end = {0}; \
  struct timespec _start = {0}; \
  clock_gettime(CLOCK_REALTIME, &_start); \
  __VA_ARGS__

#define TIMER_END() (clock_gettime(CLOCK_REALTIME, &_end), ((((_end.tv_sec - _start.tv_sec) * 1000000000.0f) + _end.tv_nsec) - (_start.tv_nsec)) / 1000000000.0f)

extern char log_entries[MAX_LOG_ENTRY][LOG_ENTRY_LENGTH];
extern u32 log_entry_count;
extern u32 log_head;

typedef struct State {
  f32 dt;
  f32 timer;
  f32 bpm;
  u32 tick;
  u32 paused;
  Camera camera;
  Node nodes[MAX_NODE];
} __attribute__((packed, aligned(sizeof(u32)))) State;

typedef struct Engine {
  // serializable state
  State state;
  // internal state
  Buffer buffer_map;
  i32 buffer_fd;
  u32 show_info_box;
  u32 show_log_box;
} Engine;

i32 signal_engine_start(i32 argc, char** argv);

void signal_engine_log(Engine* e, const char* tag, const char* format, ...);

void signal_engine_state_store(const char* path, Engine* e);

void signal_engine_state_load(const char* path, Engine* e);

#endif // _SIGNAL_ENGINE_H
