// node.h

#ifndef _NODE_H
#define _NODE_H

typedef enum {
  NODE_NONE = 0,
  NODE_CLOCK,
  NODE_ADD,
  NODE_IO,
  NODE_AND,
  NODE_PRINT,
  NODE_INCR,

  MAX_NODE_TYPE,
} Node_type;

const char* node_type_str[MAX_NODE_TYPE] = {
  [NODE_NONE]  = "none",
  [NODE_CLOCK] = "clock",
  [NODE_ADD]   = "add",
  [NODE_IO]    = "io",
  [NODE_AND]   = "and",
  [NODE_PRINT] = "print",
  [NODE_INCR]  = "incr",
};

typedef union {
  struct {
    u16 value;
  };
} Node_data;

typedef struct {
  Box box;
  Node_type type;
  Node_data data;
  u16 alive;
  u16 reads;
  u16 writes;
  u16 ready;
  u16 id;
  u32 color;
  u32 target_color;
} __attribute__((packed, aligned(sizeof(u32)))) Node;

struct Engine;
struct State;

typedef void (*node_event)(Node* self, Node* input, struct Engine*);

typedef struct {
  node_event event;
  u32 reads;
} Node_event;

void node_init(Node* node, Box box, Node_type type);

void node_reset(Node* node);

void node_clear(Node* node);

void node_grid_init(struct State* state);

void nodes_update_and_render(struct Engine* e);

void node_render_info_box(struct Engine* e, Node* node);

#endif
