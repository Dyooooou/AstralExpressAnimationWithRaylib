#ifndef STAR_H
#define STAR_H

#include "raylib.h"

#define NUM_STARS 150

typedef struct {
    float x, y, size, speed;
} Bintang;

extern Bintang bintang[NUM_STARS];
extern bool showOutline;

void initBintang(void);
void updateBintang(float warpFactor, float dt);
void drawBintang(float warpFactor, Vector2 offset);

#endif