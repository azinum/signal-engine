// node.h

#ifndef _NODE_H
#define _NODE_H

typedef enum {
  NODE_NONE = 0,
  NODE_CLOCK, // increments an internal counter and tells the neighbours
  NODE_ADDER, // add inputs from other nodes
  NODE_IO,  // when reciving input, send output
  NODE_AND, // ands inputs together, tell the neighbours if the result is non-zero, then reset the node to zero
  NODE_PRINT, // print the value(s) that this node recieved

  MAX_NODE_TYPE,
} Node_type;

const char* node_type_str[MAX_NODE_TYPE] = {
  [NODE_NONE]  = "none",
  [NODE_CLOCK] = "clock",
  [NODE_ADDER] = "adder",
  [NODE_IO]    = "io",
  [NODE_AND]   = "and",
  [NODE_PRINT] = "print",
};

const u32 node_type_color[MAX_NODE_TYPE] = {
  COLOR_GRAY,
  COLOR_RED,
  COLOR_BLUE,
  COLOR_PURPLE,
  COLOR_DARK_GREEN,
  COLOR_YELLOW
};

typedef union {
  struct {
    u32 counter;
  };
} Node_data;

typedef struct {
  Box box;
  Node_type type;
  Node_data data;
  u32 alive;
  u32 reads;
  u32 writes;
  u32 id;
  u32 color;
  u32 target_color;
} __attribute__((packed, aligned(sizeof(u32)))) Node;

struct State;

typedef void (*node_event)(Node* self, Node* input, struct State*);

typedef struct {
  node_event event;
  u32 reads;
} Node_event;

void node_init(Node* node, Box box, Node_type type);

void node_reset(Node* node);

void node_clear(Node* node);

void node_grid_init(struct State* state);

void nodes_update_and_render(struct State* state);

void node_render_info_box(struct State* state, Node* node);

#endif
