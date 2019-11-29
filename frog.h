#ifndef FROG_H
#define FROG_H

#include "utilities.h"
#include "scene.h"

typedef struct { vec3f r0, v0, r, v; } state;
typedef struct { float magnitude, angle, rotationY; } vec3fPolar;
typedef struct { 
	int stacks, slices;
	float radius;
} sphere;
typedef struct {
    float cx, cy, rx, ry;
    int num_segments;
} ellipse;

void updateProjectileInitialState(state *projectile, vec3fPolar *initVel);
void updateFrogInitialPosition(state *projectile, sphere *frog, road_o *road, bool collidedWithLogs, log_o logHit, river_o *river, float tSinWave, float log_radius);
void updateProjectileStateNumerical(state *projectile, float dt);
void displayProjectile(state *projectile, sphere *frog, bool filled, bool gameEnds, bool showAxes, bool visualizeNormal, int size, vec3f vertices_frog_body[size], vec3f vertices_frog_arm[size], vec3f vertices_frog_elbow[size]);
void displayTrajectoryNumerical(state *projectile, sphere *frog, car_o *cars, road_o *road, river_o *river, log_o logs[], float dt, float tSinWave, int sizeOfCars, int sizeOfLogs, float log_length, float log_radius);
bool trajectoryCollisionDetection(float currentX, float currentY, float currentZ, sphere *frog, car_o cars[], road_o *road, river_o *river, log_o logs[], float dt, float tSinWave, int sizeOfCars, int sizeOfLogs, float log_length, float log_radius);
void drawFrog(sphere *frog, bool showAxes, bool filled, bool visualizeNormal, int size, vec3f vertices_frog_body[size], vec3f vertices_frog_arm[size], vec3f vertices_frog_elbow[size]);
void drawCube(float length, float width, float height, bool showAxes);
void drawJumpVector(state *projectile);
bool collidedWithWave(float frogX, float frogY, float frogZ, river_o *river, float t);
bool collidedWithGround(float frogY, float frogZ, sphere *frog, river_o *river);
bool collidedWithRoad(float frogX, float frogY, float frogZ, sphere *frog, road_o *road);
bool collidedWithCars(float frogX, float frogY, float frogZ, road_o *road, sphere *frog, car_o cars[], int sizeOfCars);
int collidedWithLogs(float frogX, float frogY, float frogZ, sphere *frog, log_o logs[], river_o *river, float t, int sizeOfLogs, float log_length, float logRadius);
void respawn(state *projectile, vec3fPolar *initVel, const vec3f *respawnPos, float frogRadius);
bool gameWon(float frogZ, river_o *river);
void initCubeVertices(float length, float width, float height, int size, vec3f vertices_cube[size]); 
void drawNormCube(int size, vec3f vertices[size]);

#endif

