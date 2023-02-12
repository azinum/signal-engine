// camera.c

void camera_init(Camera* camera) {
  camera->x = 0;
  camera->y = 0;
}

void camera_update(Engine* e, Camera* camera) {
  camera->x = lerp(camera->x, camera->x_pos, CAMERA_TARGET_SPEED * e->state.dt);
  camera->y = lerp(camera->y, camera->y_pos, CAMERA_TARGET_SPEED * e->state.dt);
}
