// node.c

#define MAX_NEIGHBOUR 4

static Node* node_from_grid_pos(State* state, u32 x, u32 y);
static Result id_to_grid_pos(Node* node, u32* x, u32* y);
static void node_get_alive_neighbours(struct State* state, Node* node, Node* neighbours[MAX_NEIGHBOUR], u32* count);

Node* node_from_grid_pos(State* state, u32 x, u32 y) {
  Node* result = NULL;
  u32 index = y * NODE_GRID_WIDTH + x;
  if (index < MAX_NODE) {
    result = &state->nodes[index];
  }
  return result;
}

Result id_to_grid_pos(Node* node, u32* x, u32* y) {
  Result result = Ok;
  *x = node->id % NODE_GRID_WIDTH;
  *y = node->id / NODE_GRID_WIDTH;
  return result;
}

void node_get_alive_neighbours(struct State* state, Node* node, Node* neighbours[MAX_NEIGHBOUR], u32* count) {
  *count = 0;
  u32 x = 0;
  u32 y = 0;
  id_to_grid_pos(node, &x, &y);

  {
    Node* n = node_from_grid_pos(state, x - 1, y);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
  {
    Node* n = node_from_grid_pos(state, x + 1, y);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
  {
    Node* n = node_from_grid_pos(state, x, y - 1);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
  {
    Node* n = node_from_grid_pos(state, x, y + 1);
    if (n) {
      if (n->alive) {
        neighbours[(*count)++] = n;
      }
    }
  }
}

void node_init(Node* node, Box box, Node_type type) {
  node->box = box;
  node->type = type;
  memset(&node->data, 0, sizeof(Node_data));
  node->alive = true;
  node->color = color_black;
  node->target_color = color_black;
}

void node_grid_init(struct State* state) {
  const u32 PADDING = 2;
  const u32 NODE_WIDTH = 20;
  const u32 NODE_HEIGHT = 20;
  for (u32 y = 0; y < NODE_GRID_HEIGHT; ++y) {
    for (u32 x = 0; x < NODE_GRID_WIDTH; ++x) {
      Node* node = &state->nodes[y * NODE_GRID_WIDTH + x];
      node_init(node, BOX(PADDING + x * (NODE_WIDTH + PADDING), PADDING + y * (NODE_HEIGHT + PADDING), NODE_WIDTH, NODE_HEIGHT), NODE_CLOCK);
      node->alive = false;
      node->id = y * NODE_GRID_WIDTH + x;
    }
  }
}

void nodes_update_and_render(struct State* state) {
  const f32 bps = state->bpm / 60.0f;
  u32 beat = 0;
  if (state->timer >= (1.0f / bps)) {
    beat = 1;
    state->timer = state->timer - (1.0f / bps);
  }
  Node* hover = NULL;

  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &state->nodes[i];
    if (inside_box(&node->box, mouse_x, mouse_y)) {
      if (mouse_pressed[MOUSE_BUTTON_LEFT]) {
        node->alive = !node->alive;
      }
      if (mouse_scroll_y > 0) {
        node->type = (node->type + 1) % MAX_NODE_TYPE;
      }
      else if (mouse_scroll_y < 0) {
        if (node->type == 0) {
          node->type = MAX_NODE_TYPE - 1;
        }
        else {
          node->type -= 1;
        }
      }
      hover = node;
    }
    if (!node->alive) {
      continue;
    }
    switch (node->type) {
      case NODE_CLOCK: {
        if (beat) {
          node->data.counter++;
          u32 count = 0;
          Node* neighbours[MAX_NEIGHBOUR] = {NULL};
          node_get_alive_neighbours(state, node, neighbours, &count);
          for (u32 i = 0; i < count; ++i) {
            Node* n = neighbours[i];
            n->data.counter++;
            n->color = color_white;
          }
        }
        break;
      }
      default:
        break;
    }
  }

  for (u32 i = 0; i < MAX_NODE; ++i) {
    Node* node = &state->nodes[i];
    if (!node->alive) {
      render_rect(node->box.x, node->box.y, node->box.w, node->box.h, color_rgb(0x00, 0x00, 0x00));
      continue;
    }
    node->target_color = colors[node_type_color[node->type]];
    node->color = color_lerp(node->color, node->target_color, state->dt * 20.0f);
    if (inside_box(&node->box, mouse_x, mouse_y)) {
      node->color = color_lerp(node->target_color, color_white, 0.5f);
    }
    render_rect(node->box.x, node->box.y, node->box.w, node->box.h, node->color);
  }

  node_render_info_box(state, hover);
}

void node_render_info_box(struct State* state, Node* node) {
  u32 width = 0;
  u32 height = 0;
  platform_window_size(&width, &height);
  u32 x = 618;
  u32 y = 2;
  const u32 box_w = width - 620;
  const u32 box_h = height - 8;
  render_rect(618, 2, box_w, box_h, color_rgb(0x15, 0x16, 0x20));
  if (node) {
    render_text_format(x + 4, y + 4 + 0*20, 2, color_white, "type: %s", node_type_str[node->type], node->id);
    render_text_format(x + 4, y + 4 + 1*20, 2, color_white, "id: %d", node->id);
    render_text_format(x + 4, y + 4 + 2*20, 2, color_white, "counter: %d", node->data.counter);
  }
}
