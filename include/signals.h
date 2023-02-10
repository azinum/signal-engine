// signals.h

#ifndef _SIGNALS_H
#define _SIGNALS_H

#include "common.h"
#include "platform.h"
#include "renderer.h"

typedef struct {
  f32 dt;
  f32 total_time;
  u32 tick;
} State;

i32 signals_start(i32 argc, char** argv);

#endif // _SIGNALS_H
