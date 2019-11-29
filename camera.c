#include "camera.h"

void setPerspectiveProjection(float aspectRatio)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(75, aspectRatio, 0.1, 100.0);
}

void setupCamera(camera_g* camera, state *projectile)
{
    //  set projection matrix
    setPerspectiveProjection(camera->aspectRatio);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //  translation and rotation
    glTranslatef(0, 0, -camera->zoomFactor / 100.0); // zoom
    
    glRotatef(camera->rotation_angle_y, 0, 1, 0);
    glRotatef(camera->rotation_angle_x, 1, 0, 0);
    // follow the frog
    glTranslatef(-projectile->r.x, -projectile->r.y, -projectile->r.z);
}