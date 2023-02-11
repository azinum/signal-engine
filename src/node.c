// node.c

void node_init(Node* node, Box box, Node_type type) {
  node->box = box;
  node->type = type;
  memset(&node->data, 0, sizeof(Node_data));
  node->alive = true;
  node->color = color_black;
  node->target_color = color_rgb(0xf0, 0x22, 0x22);
}

void node_grid_init(struct State* state) {
  const u32 PADDING = 10;
  const u32 NODE_WIDTH = 26;
  const u32 NODE_HEIGHT = 26;
  for (u32 y = 0; y < NODE_GRID_HEIGHT; ++y) {
    for (u32 x = 0; x < NODE_GRID_WIDTH; ++x) {
      Node* node = &state->nodes[y * NODE_GRID_WIDTH + x];
      node_init(node, BOX(PADDING + x * (NODE_WIDTH + PADDING), PADDING + y * (NODE_HEIGHT + PADDING), NODE_WIDTH, NODE_HEIGHT), NODE_PULSE);
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
      hover = node;
    }
    if (!node->alive) {
      continue;
    }
    if (beat) {
      node->data.counter++;
      node->data.counter %= 100;
    }
    switch (node->type) {
      case NODE_PULSE: {
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
    node->color = color_lerp(node->color, node->target_color, state->dt * 32.0f);
    if (inside_box(&node->box, mouse_x, mouse_y)) {
      node->color = color_lerp(node->target_color, color_white, 0.5f);
    }
    render_rect(node->box.x, node->box.y, node->box.w, node->box.h, node->color);
    render_text_format(node->box.x, node->box.y, 2, color_white, "%d", node->data.counter);
  }

  if (hover) {
    node_render_info_box(state, hover);
  }
}

void node_render_info_box(struct State* state, Node* node) {
  const u32 box_w = 200;
  const u32 box_h = 86;
  render_rect(mouse_x + 16, mouse_y + 16, box_w, box_h, color_rgb(0xf0, 0xf0, 0xf0));
  render_text_format(mouse_x + 20, mouse_y + 20 + 0*20, 2, color_black, "id: %d", node->id);
  render_text_format(mouse_x + 20, mouse_y + 20 + 1*20, 2, color_black, "type: %d", node->type);
  render_text_format(mouse_x + 20, mouse_y + 20 + 2*20, 2, color_black, "alive: %d", node->alive);
  render_text_format(mouse_x + 20, mouse_y + 20 + 3*20, 2, color_black, "counter: %d", node->data.counter);
}
