// camera.h

#ifndef _CAMERA_
#define _CAMERA_

#define CAMERA_SPEED 600.0f
#define CAMERA_TARGET_SPEED 20.0f

typedef struct {
  f32 x;
  f32 y;
  f32 x_pos;
  f32 y_pos;
} Camera;

struct Engine;

void camera_init(Camera* camera);

void camera_update(struct Engine* e, Camera* camera);

#endif // _CAMERA_
