#ifndef PLANET_H
#define PLANET_H

#include "raylib.h"

#define NUM_PLANETS 2

typedef struct {
    float x, y;
    float radius;
    Color color;
    float speed;
} Planet;

extern Planet planets[NUM_PLANETS];

void randomizePlanets(void);
void updatePlanets(float warpFactor, float dt);
void drawPlanet(float cx, float cy, float r, Color warna);

#endif