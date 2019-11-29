#ifndef SCENE_H
#define SCENE_H

#include "utilities.h"

enum directions {left, right}; 

typedef struct road
{
    float start, width, height;
} road_o;

typedef struct river
{
    float start, width, depth;
} river_o;

typedef struct car
{
    float start, length, height, width;
    enum directions direction;
} car_o;

typedef struct log
{
    float xPos;
    enum directions direction;
} log_o;

void setLighting();
void setMaterial(const float amb_dif[3], const float specular[3], float shininess);
void drawWorld(int rows, int cols, vec3f vertices_world[rows][cols], bool showAxes, bool filled, river_o *river, GLuint texture);
void drawRoad(road_o road, bool filled, GLuint texture);
void drawGround(road_o road);
void drawRiver(river_o river, int rows, int num_segments, vec3f vertices_water[rows][num_segments], bool showAxes, float t, bool filled, GLuint texture);

void drawLogs(log_o logs[], float log_length, float r, int size, int num_segments, river_o river, int rows, int cols, vec3f vertices_logs[rows][cols], bool showAxes, float t, float dt, bool filled, GLuint texture);
void drawCars(car_o cars[], int size, road_o road, int rows, int cols, vec3f vertices_cars[rows][cols], bool showAxes, float dt, bool filled);

void drawNormWorld(int rows, int cols, vec3f vertices_world[rows][cols]);
void drawNormCars(int rows, int cols, vec3f vertices_cars[rows][cols]);
void drawNormWater(int rows, int cols, vec3f vertices_water[rows][cols], river_o river, float t);
void drawNormLogs(int rows, int cols, vec3f vertices_logs[rows][cols], river_o river, int num_segments, log_o logs[], float log_length, float r, float t);
#endif

