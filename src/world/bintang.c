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
        bintang[i].speed = 20.0f + (rand() % 60);
    }
}

void updateBintang(float warpFactor, float dt) {
    for (int i = 0; i < NUM_STARS; i++) {
        bintang[i].x -= bintang[i].speed * (1.0f + warpFactor * 10.0f) * dt;
        if (bintang[i].x < 0) bintang[i].x = SW;
    }
}

void drawBintang(float warpFactor) {
    for (int i = 0; i < NUM_STARS; i++) {
        if (showOutline) {
            Midcircle((int)bintang[i].x, (int)bintang[i].y, (int)bintang[i].size, GREEN);
        } else {
            if (warpFactor > 0.2f) {
                float streak = warpFactor * bintang[i].speed * 0.3f;
                int thick = (int)(bintang[i].size * 0.7f);
                if (thick < 1) thick = 1;
                Bres_ThickLine(
                    (int)bintang[i].x, (int)bintang[i].y,
                    (int)(bintang[i].x + streak), (int)bintang[i].y,
                    thick, (Color){200, 220, 255, 200}
                );
            } else {
                MidcircleFilled(
                    (int)bintang[i].x, (int)bintang[i].y,
                    (int)bintang[i].size, (Color){200, 220, 255, 180}
                );
            }
        }
    }
}