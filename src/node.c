// node.c

#define MAX_NEIGHBOUR 4
#define MAX_READ_EVENTS 4
#define BORDER_THICKNESS 2

Node copy_data;
Node* copy = NULL;

static Node* node_from_grid_pos(Engine* e, u32 x, u32 y);
static Result id_to_grid_pos(Node* node, u32* x, u32* y);
static void node_get_alive_neighbours(Engine* e, Node* node, Node* neighbours[MAX_NEIGHBOUR], u32* count);
static void node_copy(Node* dest, Node* src);

// broadcast to neighbours
static u32 node_broadcast(Node* node, Node* input, Engine* e);
static u32 node_increment_writes(Node* node);
static u32 node_increment_reads(Node* node);

static void event_callback(Node* node, Node* input, Engine* e);

static void event_none(Node* self, Node* input, Engine* e);
static void event_clock(Node* self, Node* input, Engine* e);
static void event_adder(Node* self, Node* input, Engine* e);
static void event_io(Node* self, Node* input, Engine* e);
static void event_and(Node* self, Node* input, Engine* e);
static void event_print(Node* self, Node* input, Engine* e);
static void event_incr(Node* self, Node* input, Engine* e);

static Node_event node_events[MAX_NODE_TYPE] = {
  { .event = event_none,  .reads = 0, },
  { .event = event_clock, .reads = 0, },
  { .event = event_adder, .reads = 2, },
  { .event = event_io,    .reads = 1, },
  { .event = event_and,   .reads = 2, },
  { .event = event_print, .reads = 1, },
  { .event = event_incr, .reads = 1, },
};

void event_callback(Node* node, Node* input, Engine* e) {
  if (!node) {
    return;
  }
  if (node->reads < MAX_READ_EVENTS && node->type < MAX_NODE_TYPE) {
    Node_event* event = &node_events[node->type];
    event->event(node, input, e);
    if (node->reads >= event->reads) {
      node->reads = 0;
      node->writes = 0;
    }
  }
}

void event_none(Node* self, Node* input, Engine* e) {
  (void)self;
  (void)input;
  (void)e;
  if (input) {
    node_increment_reads(self);
  }
}

void event_clock(Node* self, Node* input, Engine* e) {
  if (input) {
    return;
  }
  self->data.counter++;
  node_broadcast(self, input, e);
}

void event_adder(Node* self, Node* input, Engine* e) {
  if (!input) {
    return;
  }
  self->data.counter += input->data.counter;
  if (node_increment_reads(self) == 2) {
    node_broadcast(self, input, e);
  }
}

void event_io(Node* self, Node* input, Engine* e) {
  if (input) {
    node_increment_reads(self);
    self->data = input->data;
  }
  node_broadcast(self, input, e);
}

void event_and(Node* self, Node* input, Engine* e) {
  if (!input) {
    return;
  }
  node_increment_reads(self);
  self->data.counter += input->data.counter != 0;

  if (self->data.counter >= 2) {
    self->data.counter = 1;
    node_broadcast(self, input, e);
    self->data.counter = 0;
  }
}

void event_print(Node* self, Node* input, Engine* e) {
  if (!input) {
    return;
  }
  node_increment_reads(self);
  self->data = input->data;
  printf("%04d:%s: %u\n", self->id, node_type_str[self->type], self->data.counter);
}

void event_incr(Node* self, Node* input, Engine* e) {
  if (!input) {
    return;
  }
  self->data.counter += input->data.counter;
  node_broadcast(self, input, e);
}

Node* node_from_grid_pos(Engine* e, u32 x, u32 y) {
  Node* result = NULL;
  u32 index = y * NODE_GRID_WIDTH + x;
  if (index < MAX_NODE) {
    result = &e->state.nodes[index];
  }
  return result;
}

Result id_to_grid_pos(Node* node, u32* x, u32* y) {
  Result result = Ok;
  *x = node->id % NODE_GRID_WIDTH;
  *y = node->id / NODE_GRID_WIDTH;
  return result;
}

void node_get_alive_neighbours(Engine* e, Node* node, Node* neighbours[MAX_NEIGHBOUR], u32* count) {
  *count = 0;
  u32 x = 0;
  u32 y = 0;
  id_to_grid_pos(node, &x, &y);

  {
    Node* n = node_from_grid_pos(e, x - 1, y);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
  {
    Node* n = node_from_grid_pos(e, x + 1, y);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
  {
    Node* n = node_from_grid_pos(e, x, y - 1);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
  {
    Node* n = node_from_grid_pos(e, x, y + 1);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
}

void node_copy(Node* dest, Node* src) {
  node_reset(dest);
  dest->type = src->type;
  dest->data = src->data;
}

u32 node_broadcast(Node* self, Node* input, Engine* e) {
  u32 count = 0;
  Node* neighbours[MAX_NEIGHBOUR] = {NULL};
  node_get_alive_neighbours(e, self, neighbours, &count);
  for (u32 i = 0; i < count; ++i) {
    Node* n = neighbours[i];
    if (n == input) { // don't loop back
      continue;
    }
    node_increment_writes(self);
    event_callback(n, self, e);
  }
  return count;
}

u32 node_increment_writes(Node* node) {
  node->color = colors[COLOR_RED];
  return ++node->writes;
}

u32 node_increment_reads(Node* node) {
  node->color = colors[COLOR_GREEN];
  return ++node->reads;
}

void node_init(Node* node, Box box, Node_type type) {
  node->box = box;
  node->type = type;
  memset(&node->data, 0, sizeof(Node_data));
  node->alive = true;
  node->reads = 0;
  node->writes = 0;
  node->color = color_black;
  node->target_color = color_black;
}

void node_reset(Node* node) {
  node_init(node, node->box, node->type);
}

void node_clear(Node* node) {
  node_init(node, node->box, NODE_NONE);
}

void node_grid_init(State* state) {
  const u32 PADDING = DEFAULT_PADDING;
  const u32 NODE_WIDTH = 20;
  const u32 NODE_HEIGHT = 20;
  for (u32 y = 0; y < NODE_GRID_HEIGHT; ++y) {
    for (u32 x = 0; x < NODE_GRID_WIDTH; ++x) {
      Node* node = &state->nodes[y * NODE_GRID_WIDTH + x];
      node_init(node, BOX(PADDING + x * (NODE_WIDTH + PADDING), PADDING + y * (NODE_HEIGHT + PADDING), NODE_WIDTH, NODE_HEIGHT), NODE_NONE);
      node->alive = false;
      node->id = y * NODE_GRID_WIDTH + x;
    }
  }
}

void nodes_update_and_render(Engine* e) {
  const f32 bps = e->state.bpm / 60.0f;
  u32 beat = 0;
  if (e->state.timer >= (1.0f / bps)) {
    beat = 1;
    e->state.timer = e->state.timer - (1.0f / bps);
  }
  Node* hover = NULL;

  // process nodes
  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &e->state.nodes[i];
    if (inside_box(&node->box, mouse_x, mouse_y)) {
      if (mouse_pressed[MOUSE_BUTTON_RIGHT]) {
        node_clear(node);
        node->alive = false;
        continue;
      }
      if (mouse_scroll_y > 0) {
        node_reset(node);
        node->type = (node->type + 1) % MAX_NODE_TYPE;
        continue;
      }
      else if (mouse_scroll_y < 0) {
        node_reset(node);
        if (node->type == 0) {
          node->type = MAX_NODE_TYPE - 1;
        }
        else {
          node->type -= 1;
        }
        continue;
      }
      if (key_pressed[KEY_V] && key_mod_ctrl) {
        if (copy) {
          node_copy(node, copy);
          log_info("pasted node `%d`\n", copy->id);
        }
      }
      hover = node;
    }
    if (!node->alive) {
      continue;
    }
    if (key_pressed[KEY_Q]) {
      node_reset(node);
      continue;
    }
    if (beat && node->type == NODE_CLOCK) {
      event_callback(node, NULL, e);
    }
  }

  if (hover) {
    if (mouse_pressed[MOUSE_BUTTON_LEFT]) {
      hover->data.counter = 1;
      event_callback(hover, NULL, e);
    }
    if (key_pressed[KEY_R]) {
      node_reset(hover);
    }
    if (key_pressed[KEY_C] && key_mod_ctrl) {
      copy = &copy_data;
      memcpy(copy, hover, sizeof(Node));
      log_info("copied node `%d`\n", hover->id);
    }
    if (key_pressed[KEY_X] && key_mod_ctrl) {
      copy = &copy_data;
      memcpy(copy, hover, sizeof(Node));
      node_clear(hover);
      hover->alive = false;
      log_info("cut node `%d`\n", hover->id);
    }
  }

  // and finally render them
  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &e->state.nodes[i];
    if (!node->alive) {
      render_fill_rect(node->box.x, node->box.y, node->box.w, node->box.h, color_rgb(0x00, 0x00, 0x00));
      continue;
    }
    node->color = color_lerp(node->color, node->target_color, e->state.dt * 10.0f);

    if (node == hover) {
      node->color = color_lerp(node->target_color, color_white, 1.0f);
    }
    render_sprite_from_id(node->box.x, node->box.y, node->box.w, node->box.h, (Sprite_id)node->type);
    render_rect(node->box.x, node->box.y, node->box.w, node->box.h, BORDER_THICKNESS, node->color);
  }

  node_render_info_box(e, hover);
}

void node_render_info_box(Engine* e, Node* node) {
  u32 width = 0;
  u32 height = 0;
  platform_window_size(&width, &height);
  u32 x = 618;
  u32 y = 2;
  const u32 PADDING = DEFAULT_PADDING;
  const u32 box_w = width - 620;
  const u32 box_h = height - (2 * PADDING);
  render_fill_rect(618, 2, box_w, box_h, color_rgb(0x15, 0x16, 0x20));
  if (node) {
    render_text_format(x + PADDING, y + PADDING + 0*20, 2, color_white, "type: %s", node_type_str[node->type], node->id);
    render_text_format(x + PADDING, y + PADDING + 1*20, 2, color_white, "id: %u", node->id);
    render_text_format(x + PADDING, y + PADDING + 2*20, 2, color_white, "counter: %u", node->data.counter);
    render_text_format(x + PADDING, y + PADDING + 3*20, 2, color_white, "reads: %u", node->reads);
    render_text_format(x + PADDING, y + PADDING + 4*20, 2, color_white, "writes: %u", node->writes);
    render_sprite_from_id(x + PADDING, y + PADDING + 5*20, 84, 84, (Sprite_id)node->type);
    render_rect(x + PADDING, y + PADDING + 5*20, 84, 84, BORDER_THICKNESS, color_white);
  }
  if (copy) {
    render_text_format(x + PADDING, y + PADDING + 11*20, 2, color_white, "clipboard");
    render_sprite_from_id(x + PADDING, y + PADDING + 12*20, 84, 84, (Sprite_id)copy->type);
    render_rect(x + PADDING, y + PADDING + 12*20, 84, 84, BORDER_THICKNESS, color_white);
  }
}
