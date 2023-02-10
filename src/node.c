// node.c

void node_init(Node* node, Box box, Node_type type) {
  node->box = box;
  node->type = type;
  node->state = ALIVE;
  node->id = 0;
  node->color = color_black;
  node->target_color = color_rgb(0xf0, 0x22, 0x22);
}

void node_push(State* state, Node* node) {
  if (state->node_count < MAX_NODE) {
    node->id = ++state->id;
    memcpy(&state->nodes[state->node_count++], node, sizeof(Node));
  }
}

void nodes_update_and_render(struct State* state) {
  for (u32 i = 0; i < state->node_count; ++i) {
    Node* node = &state->nodes[i];
    if (node->state == DEAD) {
      continue;
    }

    switch (node->type) {
      case NODE_PULSE: {
        break;
      }
      default:
        break;
    }
  }

  for (u32 i = 0; i < state->node_count; ++i) {
    Node* node = &state->nodes[i];
    if (node->state == DEAD) {
      continue;
    }
    node->color = color_lerp(node->color, node->target_color, state->dt * 20.0f);
    if (inside_box(&node->box, mouse_x, mouse_y)) {
      node->color = color_lerp(node->color, color_white, 0.1f);
    }
    render_rect(node->box.x, node->box.y, node->box.w, node->box.h, node->color);
  }
}
