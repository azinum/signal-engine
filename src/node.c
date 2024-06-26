// node.c

#define BORDER_THICKNESS 2

#define MAX_NEIGHBOUR 4
#define MAX_READS 4
#define MAX_WRITES 4

static Node copy_data;
static Node* copy = NULL;

static Node* node_from_grid_pos(Engine* e, u32 x, u32 y);
static Result id_to_grid_pos(Node* node, u32* x, u32* y);
static Result node_to_grid_pos(Node* node, u32* x, u32* y);
static void node_get_alive_neighbours(Engine* e, Node* node, Node* neighbours[MAX_NEIGHBOUR], u32* count);
static Node* node_get_alive_neighbour(Engine* e, Node* node, i32 delta_x, i32 delta_y);
static void node_copy(Node* dest, Node* src);

// broadcast to neighbours
static u32 node_broadcast(Node* node, Node* input, Engine* e);
static u16 node_increment_writes(Node* self);
static u16 node_increment_reads(Node* self);
static u32 node_safe_guard(Node* self);
static u32 node_finalize(Node* self);

static void node_event_callback(Node* node, Node* input, Engine* e);

static void node_event_none(Node* self, Node* input, Engine* e);
static void node_event_clock(Node* self, Node* input, Engine* e);
static void node_event_add(Node* self, Node* input, Engine* e);
static void node_event_bus(Node* self, Node* input, Engine* e);
static void node_event_and(Node* self, Node* input, Engine* e);
static void node_event_print(Node* self, Node* input, Engine* e);
static void node_event_incr(Node* self, Node* input, Engine* e);
static void node_event_not(Node* self, Node* input, Engine* e);
static void node_event_copy(Node* self, Node* input, Engine* e);
static void node_event_equals(Node* self, Node* input, Engine* e);
static void node_event_copy_lr(Node* self, Node* input, Engine* e);
static void node_event_copy_rl(Node* self, Node* input, Engine* e);
static void node_event_copy_ud(Node* self, Node* input, Engine* e);
static void node_event_copy_du(Node* self, Node* input, Engine* e);

static void node_broadcast_event_copy(Node* self, Node* input, Engine* e);

static Node_event node_events[MAX_NODE_TYPE] = {
  [NODE_NONE]    = { .event = node_event_none,    .broadcast = NULL, .reads = 0, },
  [NODE_CLOCK]   = { .event = node_event_clock,   .broadcast = NULL, .reads = 0, },
  [NODE_ADD]     = { .event = node_event_add,     .broadcast = NULL, .reads = 2, },
  [NODE_BUS]     = { .event = node_event_bus,     .broadcast = NULL, .reads = 1, },
  [NODE_AND]     = { .event = node_event_and,     .broadcast = NULL, .reads = 2, },
  [NODE_PRINT]   = { .event = node_event_print,   .broadcast = NULL, .reads = 1, },
  [NODE_INCR]    = { .event = node_event_incr,    .broadcast = NULL, .reads = 1, },
  [NODE_NOT]     = { .event = node_event_not,     .broadcast = NULL, .reads = 1, },
  [NODE_COPY]    = { .event = node_event_copy,    .broadcast = node_broadcast_event_copy, .reads = 1, },
  [NODE_EQUALS]  = { .event = node_event_equals,  .broadcast = NULL, .reads = 2, },
  [NODE_COPY_LR] = { .event = node_event_copy_lr, .broadcast = NULL, .reads = 1, },
  [NODE_COPY_RL] = { .event = node_event_copy_rl, .broadcast = NULL, .reads = 1, },
  [NODE_COPY_UD] = { .event = node_event_copy_ud, .broadcast = NULL, .reads = 1, },
  [NODE_COPY_DU] = { .event = node_event_copy_du, .broadcast = NULL, .reads = 1, },
};

void node_event_callback(Node* node, Node* input, Engine* e) {
  if (!node) {
    return;
  }
  if (!node->ready) {
    return;
  }
  assert(node->type < MAX_NODE_TYPE);
  Node_event* event = &node_events[node->type];
  if (node->reads < event->reads && node->writes < MAX_WRITES) {
    event->event(node, input, e);
    return;
  }
  if (event->reads == 0 && node->writes < MAX_WRITES) {
    event->event(node, NULL, e);
  }
}

void node_event_none(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  node_increment_reads(self);
  node_finalize(self);
}

void node_event_clock(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  self->data.value++;
  node_broadcast(self, input, e);
  node_finalize(self);
}

void node_event_add(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }

  u16 reads = node_increment_reads(self);
  self->data.value += input->data.value;
  if (reads == 2) {
    node_broadcast(self, input, e);
  }
  else if (reads > 2) {
    assert(0);
  }
  node_finalize(self);
}

void node_event_bus(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    node_increment_reads(self);
  }
  node_broadcast(self, input, e);
  node_finalize(self);
}

void node_event_and(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  u16 reads = node_increment_reads(self);
  if (reads == 1) {
    self->data.value = input->data.value;
  }
  else if (reads == 2) {
    self->data.value = self->data.value && input->data.value;
    if (self->data.value) {
      node_broadcast(self, input, e);
    }
  }
  else {
    assert(0);
  }
  node_finalize(self);
}

void node_event_print(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    node_increment_reads(self);
    self->data = input->data;
    signal_engine_log(e, "node", "%s: %u", node_type_str[input->type], self->data.value);
    log_info("%s: %u\n", node_type_str[input->type], self->data.value);
  }
  node_finalize(self);
}

void node_event_incr(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    node_increment_reads(self);
    self->data.value += input->data.value;
    node_broadcast(self, input, e);
  }
  node_finalize(self);
}

void node_event_not(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    node_increment_reads(self);
    self->data.value = !input->data.value;
    node_broadcast(self, input, e);
  }
  node_finalize(self);
}

void node_event_copy(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    node_increment_reads(self);
    self->data.value = input->data.value;
    node_broadcast(self, input, e);
  }
  node_finalize(self);
}

void node_event_equals(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    u16 reads = node_increment_reads(self);
    if (reads == 1) {
      self->data.value = input->data.value;
    }
    else if (reads == 2) {
      self->data.value = self->data.value == input->data.value;
      if (self->data.value != 0) {
        node_broadcast(self, input, e);
      }
    }
    else {
      assert(0);
    }
  }
  node_finalize(self);
}

void node_event_copy_lr(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    Node* in = node_get_alive_neighbour(e, self, -1, 0);
    Node* out = node_get_alive_neighbour(e, self, 1, 0);
    if (in == input) {
      node_increment_reads(self);
      self->data.value = input->data.value;
      if (out) {
        node_increment_writes(self);
        out->data.value = self->data.value;
        node_event_callback(out, self, e);
      }
    }
  }
  node_finalize(self);
}

void node_event_copy_rl(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    Node* in = node_get_alive_neighbour(e, self, 1, 0);
    Node* out = node_get_alive_neighbour(e, self, -1, 0);
    if (in == input) {
      node_increment_reads(self);
      self->data.value = input->data.value;
      if (out) {
        node_increment_writes(self);
        out->data.value = self->data.value;
        node_event_callback(out, self, e);
      }
    }
  }
  node_finalize(self);
}

void node_event_copy_ud(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    Node* in = node_get_alive_neighbour(e, self, 0, -1);
    Node* out = node_get_alive_neighbour(e, self, 0, 1);
    if (in == input) {
      node_increment_reads(self);
      self->data.value = input->data.value;
      if (out) {
        node_increment_writes(self);
        out->data.value = self->data.value;
        node_event_callback(out, self, e);
      }
    }
  }
  node_finalize(self);
}

void node_event_copy_du(Node* self, Node* input, Engine* e) {
  if (!node_safe_guard(self)) {
    return;
  }
  if (input) {
    Node* in = node_get_alive_neighbour(e, self, 0, 1);
    Node* out = node_get_alive_neighbour(e, self, 0, -1);
    if (in == input) {
      node_increment_reads(self);
      self->data.value = input->data.value;
      if (out) {
        node_increment_writes(self);
        out->data.value = self->data.value;
        node_event_callback(out, self, e);
      }
    }
  }
  node_finalize(self);
}

void node_broadcast_event_copy(Node* self, Node* input, Engine* e) {
  assert(self && input);
  input->data.value = self->data.value;
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
  if (x) {
    *x = node->id % NODE_GRID_WIDTH;
  }
  if (y) {
    *y = node->id / NODE_GRID_WIDTH;
  }
  return result;
}

Result node_to_grid_pos(Node* node, u32* x, u32* y) {
  return id_to_grid_pos(node, x, y);
}

void node_get_alive_neighbours(Engine* e, Node* node, Node* neighbours[MAX_NEIGHBOUR], u32* count) {
  if (!node) {
    return;
  }

  assert(count != NULL);

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

Node* node_get_alive_neighbour(Engine* e, Node* node, i32 delta_x, i32 delta_y) {
  Node* result = NULL;
  u32 x = 0;
  u32 y = 0;
  node_to_grid_pos(node, &x, &y);
  Node* n = node_from_grid_pos(e, x + delta_x, y + delta_y);
  if (n) {
    if (n->alive) {
      result = n;
    }
  }
  return result;
}

void node_copy(Node* dest, Node* src) {
  node_reset(dest);
  dest->type = src->type;
  dest->data = src->data;
}

u32 node_broadcast(Node* self, Node* input, Engine* e) {
  if (!self->ready) {
    return 0;
  }
  assert(self->type < MAX_NODE_TYPE);

  Node_event* event = &node_events[self->type];

  u32 count = 0;
  Node* neighbours[MAX_NEIGHBOUR] = {NULL};
  assert(self != NULL);
  node_get_alive_neighbours(e, self, neighbours, &count);
  for (u32 i = 0; i < count; ++i) {
    Node* n = neighbours[i];
    if (n == input) { // don't loop back
      continue;
    }
    node_increment_writes(self);
    if (event->broadcast) {
      event->broadcast(self, n, e);
    }
    node_event_callback(n, self, e);
  }
  return count;
}

u16 node_increment_writes(Node* self) {
  u16 writes = ++self->writes;
  self->color = colors[COLOR_RED];
  return writes;
}

u16 node_increment_reads(Node* self) {
  u16 reads = ++self->reads;
  self->color = colors[COLOR_GREEN];
  return reads;
}

u32 node_safe_guard(Node* self) {
  assert(self->type < MAX_NODE_TYPE);
  Node_event* event = &node_events[self->type];
  if (event->reads == 0) {
    return self->ready && self->writes < MAX_WRITES;
  }
  return self->ready && self->reads < event->reads && self->writes < MAX_WRITES;
}

u32 node_finalize(Node* self) {
  assert(self->type < MAX_NODE_TYPE);
  Node_event* event = &node_events[self->type];
  if (self->reads >= event->reads || self->writes >= MAX_WRITES) {
    self->ready = false; // we're done processing this node
  }
  return 0;
}

void node_init(Node* node, Box box, Node_type type) {
  node->box = box;
  node->type = type;
  memset(&node->data, 0, sizeof(Node_data));
  node->alive = true;
  node->reads = 0;
  node->writes = 0;
  node->ready = true;
  node->color = colors[COLOR_BLACK];
  node->target_color = colors[COLOR_BLACK];
}

void node_reset(Node* node) {
  node_init(node, node->box, node->type);
}

void node_clear(Node* node) {
  node_init(node, node->box, NODE_NONE);
}

void node_grid_init(State* state) {
  const u32 PADDING = 2;
  const u32 NODE_WIDTH = 38;
  const u32 NODE_HEIGHT = 38;
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
  Camera* camera = &e->state.camera;

  // make nodes ready
  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &e->state.nodes[i];
    if (!node->ready && beat) {
      node->ready = true;
      node->reads = 0;
      node->writes = 0;
    }
  }

  // process nodes
  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &e->state.nodes[i];
    if (inside_box(&node->box, mouse_x + camera->x, mouse_y + camera->y)) {
      if (mouse_pressed[MOUSE_BUTTON_RIGHT]) {
        continue;
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
      node->ready = true;
      node_event_callback(node, NULL, e);
    }
  }

  if (hover) {
    if (mouse_pressed[MOUSE_BUTTON_LEFT]) {
      hover->data.value = 1;
      node_event_callback(hover, NULL, e);
    }
    if (key_pressed[KEY_R]) {
      node_reset(hover);
    }
    if (key_mod_ctrl) {
      if (key_pressed[KEY_C]) {
        copy = &copy_data;
        memcpy(copy, hover, sizeof(Node));
        signal_engine_log(e, "info", "copied node %u", hover->id);
      }
      if (key_pressed[KEY_X]) {
        copy = &copy_data;
        memcpy(copy, hover, sizeof(Node));
        node_clear(hover);
        hover->alive = false;
        signal_engine_log(e, "info", "cut node %u", hover->id);
      }
      if (key_pressed[KEY_V]) {
        if (copy) {
          node_copy(hover, copy);
          signal_engine_log(e, "info", "pasted node %u", copy->id);
        }
      }
      if (mouse_scroll_y > 0) {
        hover->data.value += 1;
      }
      else if (mouse_scroll_y < 0) {
        hover->data.value -= 1;
      }
    }
    else {
      if (mouse_scroll_y > 0) {
        node_reset(hover);
        hover->type = (hover->type + 1) % MAX_NODE_TYPE;
      }
      else if (mouse_scroll_y < 0) {
        node_reset(hover);
        if (hover->type == 0) {
          hover->type = MAX_NODE_TYPE - 1;
        }
        else {
          hover->type -= 1;
        }
      }
    }
  }

  // and finally render them
  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &e->state.nodes[i];
    if (!node->alive) {
      render_rect(node->box.x - camera->x, node->box.y - camera->y, node->box.w, node->box.h, BORDER_THICKNESS, node->color);
      continue;
    }
    node->color = color_lerp(node->color, node->target_color, e->state.dt * 10.0f);

    if (node == hover) {
      node->color = color_lerp(node->target_color, colors[COLOR_WHITE], 1.0f);
    }
    render_sprite_from_id(node->box.x - camera->x, node->box.y - camera->y, node->box.w, node->box.h, (Sprite_id)node->type);
    render_rect(node->box.x - camera->x, node->box.y - camera->y, node->box.w, node->box.h, BORDER_THICKNESS, node->color);
  }

  node_render_info_box(e, hover);
}

void node_render_info_box(Engine* e, Node* node) {
  u32 width = 0;
  u32 height = 0;
  u32 glyph_size = DEFAULT_GLYPH_SIZE;
  u32 padding = 2;
  platform_window_size(&width, &height);

  u32 glyph_spacing = 18;
  u32 y = 0;
  u32 placement_count = 0;

  // hacks! would be great with a proper ui
#define Y_PLACE(OFFSET) (y = (placement_count * glyph_spacing) + OFFSET, ++placement_count, y)
#define Y_PLACE_GET(OFFSET) (y = (placement_count * glyph_spacing) + OFFSET)

  if (e->show_info_box) {
    if (node) {
      if (node->alive) {
        u32 y_pos = Y_PLACE(0);
        render_fill_rect(0, y_pos, width, glyph_spacing, colors[COLOR_BLACK]);
        render_text_format(
          padding,
          y_pos + padding,
          glyph_size,
          colors[COLOR_WHITE],
          "type: %s, "
          "value: %u, "
          "reads: %u, "
          "writes: %u"
          ,
          node_type_str[node->type],
          node->data.value,
          node->reads,
          node->writes
        );
      }
    }
    if (copy) {
      if (copy->type != NODE_NONE) {
        u32 y_pos = Y_PLACE(0);
        render_fill_rect(0, y_pos, width, glyph_spacing, colors[COLOR_DARK_GRAY]);
        render_text_format(
          padding,
          y_pos + padding,
          glyph_size,
          colors[COLOR_WHITE],
          "clipboard: %s"
          ,
          node_type_str[copy->type]
        );
        render_sprite_from_id(0, Y_PLACE_GET(0), 64, 64, (Sprite_id)copy->type);
      }
    }
  }
  if (e->show_log_box) {
    u32 log_height = glyph_spacing * MAX_LOG_ENTRY;
    u32 log_width = width >> 1;
    u32 y_pos = Y_PLACE(0);
    render_fill_rect(width - log_width, y_pos, log_width, log_height, colors[COLOR_DARK_GRAY]);
    u32 index = log_head;
    for (u32 i = 0; i < log_entry_count; ++i, ++index) {
      if (index >= log_entry_count) {
        index = 0;
      }
      char* entry = log_entries[index];
      render_text_format(width - log_width, y_pos, 1, colors[COLOR_WHITE], "%s", entry);
      y_pos = Y_PLACE(0);
    }
  }

  if (e->state.paused) {
    render_text_format(padding, height - glyph_spacing, glyph_size, colors[COLOR_WHITE], "paused");
  }
}
