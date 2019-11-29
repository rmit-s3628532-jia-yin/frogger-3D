#ifndef CAMERA_H
#define CAMERA_H

#include "utilities.h"
#include "frog.h"

typedef struct {
    float lastX;
    float rotation_angle_y;
    float lastY;
    float rotation_angle_x;
    int buttonPressed;
    int zoomFactor;
    float aspectRatio;
} camera_g;

void setPerspectiveProjection(float aspectRatio);
void setupCamera(camera_g* camera, state *projectile);

#endif