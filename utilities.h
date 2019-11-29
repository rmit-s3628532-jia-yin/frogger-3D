#ifndef UTILITIES_H
#define UTILITIES_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#if _WIN32
#   include <Windows.h>
#endif
#if __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

typedef struct { float x, y; } vec2f;
typedef struct vec3f { float x, y, z; } vec3f;
typedef struct { float radius; int segments; } circle;

#define MILLI 1000

void drawAxes(float length);
float degToRad(float degree);
float radToDeg(float rad);
void drawVector(float a, float b, float c, float x, float y, float z, float scale, bool normalized);

#endif

