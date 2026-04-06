#include "bintang.h"
#include "../core/state.h"
#include "../algo/bresenham.h"
#include "../algo/midcircle.h"
#include <stdlib.h>

Bintang bintang[NUM_STARS];
bool showOutline = false;

void initBintang(void) {
    for (int i = 0; i < NUM_STARS; i++) {
        bintang[i].x     = rand() % SW;
        bintang[i].y     = rand() % SH;
        bintang[i].size  = 1.0f + (rand() % 3);
        // if (bintang[i].size > 2.5f) {
        //     bintang[i].speed = 20.0f + (rand() % 60);
        // } else {
        //     bintang[i].speed = 2.5f + (rand() % 60);
        // }
        bintang[i].speed = 20.0f + (rand() % 60);
    }
}

void updateBintang(float warpFactor, float dt) {
    for (int i = 0; i < NUM_STARS; i++) {
        bintang[i].x -= bintang[i].speed * (1.0f + warpFactor * 10.0f) * dt;
        if (bintang[i].x < 0) bintang[i].x = SW;
    }
}

void drawBintang(float warpFactor, Vector2 offset) {
    for (int i = 0; i < NUM_STARS; i++) {
        float cx = bintang[i].x + offset.x;
        float cy = bintang[i].y + offset.y;
        if (showOutline) {
            Midcircle((int)cx, (int)cy, (int)bintang[i].size, GREEN);
        } else {
            if (warpFactor > 0.2f) {
                float streak = warpFactor * bintang[i].speed * 20.0f;
                int thick = (int)(bintang[i].size * 0.7f);
                if (thick < 1) thick = 1;
                int endX = (int)(cx - streak);
                if (endX < 0) endX = 0;
                Bres_ThickLine(
                    (int)cx, (int)cy,
                    endX, (int)cy,
                    thick, (Color){200, 220, 255, 200}
                );
            } else {
                MidcircleFilled(
                    (int)cx, (int)cy,
                    (int)bintang[i].size, (Color){200, 220, 255, 180}
                );
            }
        }
    }
}