#ifndef GAME_H
#define GAME_H

#include "scene.h"
#include "frog.h"
#include "camera.h"
#include "SOIL2.h"

typedef enum {
    analytical,
    numerical
} integrationMode;

typedef struct {
    bool debug;
    bool go;
    bool respawned;
    float startTime;
    integrationMode integrateMode;
    bool OSD;
    int frames;
    float frameRate;
    float frameRateInterval;
    float lastFrameRateT;
    float dtScene;
    int logHit;
    bool end;
    int num_segments;
    bool filled;
    bool animationOff;
    bool visualizeNormal;
    bool showAxes;
    float tSinWave;
    bool lighting;
    int lives_left;
    int score;
    bool gameOver;
    bool texture;
} global_t;

void displayOSD();

#endif

