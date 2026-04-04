#ifndef KERETA_H
#define KERETA_H

#include "raylib.h"

void drawRoda(float cx, float cy, float r, float angle, Color color);
void drawKereta(float cx, float cy, float angle, float warpFactor, float rodaTheta);

#endif