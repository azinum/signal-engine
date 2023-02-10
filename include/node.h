// node.h

#ifndef _NODE_H
#define _NODE_H

typedef enum {
  NODE_NONE,
  NODE_PULSE,
} Node_type;

typedef enum {
  DEAD,
  ALIVE,
} Node_state;

typedef struct {
  Box box;
  Node_type type;
  Node_state state;
  u32 id;
  u32 color;
  u32 target_color;
} Node;

struct State;

void node_init(Node* node, Box box, Node_type type);

void node_push(struct State* state, Node* node);

void nodes_update_and_render(struct State* state);

#endif
