// node.h

#ifndef _NODE_H
#define _NODE_H

typedef enum {
  NODE_NONE = 0,
  NODE_CLOCK,

  MAX_NODE_TYPE,
} Node_type;

const char* node_type_str[MAX_NODE_TYPE] = {
  [NODE_NONE] = "none",
  [NODE_CLOCK] = "clock",
};

typedef struct {
  u32 counter;
} Node_data;

typedef struct {
  Box box;
  Node_type type;
  Node_data data;
  u32 alive;
  u32 id;
  u32 color;
  u32 target_color;
} __attribute__((packed, aligned(sizeof(u32)))) Node;

struct State;

void node_init(Node* node, Box box, Node_type type);

void node_grid_init(struct State* state);

void nodes_update_and_render(struct State* state);

void node_render_info_box(struct State* state, Node* node);

#endif
