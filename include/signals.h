// signals.h

#ifndef _SIGNALS_H
#define _SIGNALS_H

#include "common.h"
#include "platform.h"
#include "renderer.h"
#include "node.h"

#define MAX_NODE 128

typedef struct State {
  f32 dt;
  f32 total_time;
  u32 tick;
  Node nodes[MAX_NODE];
  u32 node_count;
  u32 id;
} State;

i32 signals_start(i32 argc, char** argv);

#endif // _SIGNALS_H
