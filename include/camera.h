// camera.h

#ifndef _CAMERA_
#define _CAMERA_

#define CAMERA_SPEED 600.0f
#define CAMERA_TARGET_SPEED 20.0f

typedef struct {
  f32 x;
  f32 y;
  f32 target_x;
  f32 target_y;
} Camera;

struct Engine;

void camera_init(Camera* camera);

void camera_update(struct Engine* e, Camera* camera);

#endif // _CAMERA_
