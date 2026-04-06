#ifndef KERETA_H
#define KERETA_H

#include "raylib.h"

#define NUM_TRAIL_PARTICLES 100
typedef struct {
    float x;
    float y;
    float status;
    float size;
    bool active;
} TrailParticle;


void drawRoda(float cx, float cy, float r, float angle, Color color);
void drawKereta(float cx, float cy, float angle, float warpFactor, float rodaTheta);
void drawTrail(Vector2 offset);
void spawnTrailParticle(float rx, float ry, float warpFactor);
void updateTrail(float warpFactor, float dt);
void initTrail();

#endif