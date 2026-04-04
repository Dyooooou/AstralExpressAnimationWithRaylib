#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

float lerpF(float a, float b, float t);
float clamp01(float t);
Color MyColorLerp(Color c1, Color c2, float t);

#endif