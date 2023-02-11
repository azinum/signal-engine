// signals.h

#ifndef _SIGNALS_H
#define _SIGNALS_H

#include "common.h"
#include "platform.h"
#include "renderer.h"
#include "node.h"

#define NODE_GRID_WIDTH 28
#define NODE_GRID_HEIGHT 25
#define MAX_NODE (NODE_GRID_WIDTH * NODE_GRID_HEIGHT)

#define DEFAULT_PADDING 2

#define STATE_PATH "state.bin"

typedef struct State {
  f32 dt;
  f32 timer;
  f32 bpm;
  u32 tick;
  u32 paused;
  Node nodes[MAX_NODE];
} __attribute__((packed, aligned(sizeof(u32)))) State;

typedef struct Engine {
  // serializable state
  State state;
  // internal state
  Buffer buffer_map;
  i32 buffer_fd;
} Engine;

i32 signal_engine_start(i32 argc, char** argv);

void signal_engine_state_store(const char* path, State* state);

void signal_engine_state_load(const char* path, State* state);

#endif // _SIGNALS_H
