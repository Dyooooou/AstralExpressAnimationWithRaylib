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
        int layer = rand() % 100;
        if (layer < 60) {
            // Lapisan Jauh: Bintang paling banyak (60%), ukuran kecil, gerak sangat lambat
            bintang[i].size  = 1.0f;
            bintang[i].speed = 5.0f + (rand() % 10);
        } else if (layer < 90) {
            // Lapisan Tengah: Bintang sedang (30%), ukuran medium, gerak lumayan cepat
            bintang[i].size  = 2.0f;
            bintang[i].speed = 25.0f + (rand() % 20);
        } else {
            // Lapisan Dekat: Bintang sedikit (10%), ukuran besar, gerak paling cepat
            bintang[i].size  = 3.0f;
            bintang[i].speed = 60.0f + (rand() % 40);
        }
    }
}

void updateBintang(float warpFactor, float dt) {
    for (int i = 0; i < NUM_STARS; i++) {
        float currentSpeed = bintang[i].speed * (1.0f + (warpFactor * 40.0f));
        bintang[i].x -= currentSpeed * dt;
        if (bintang[i].x < 0) {
            bintang[i].x = SW + (rand() % 50);
            bintang[i].y = rand() % SH;
        }
    }
}

void drawBintang(float warpFactor, Vector2 offset) {
    for (int i = 0; i < NUM_STARS; i++) {
        // Terapkan offset untuk efek camera shake pada setiap bintang
        float cx = bintang[i].x + offset.x;
        float cy = bintang[i].y + offset.y;

        if (showOutline) {
            Midcircle((int)cx, (int)cy, (int)bintang[i].size, GREEN);
        } else {
            if (warpFactor > 0.05f) {
                float streakMultiplier = 10.0f + (warpFactor * warpFactor * 300.0f);
                float streak = warpFactor * bintang[i].speed * streakMultiplier;
                
                int thick = (int)(bintang[i].size * 0.8f);
                if (thick < 1) thick = 1;
                int endX = (int)(cx + streak); 
                if (endX > SW) endX = SW; 
                if (cx < 0) cx = 0;
                Color starColor = (Color){200, 220, 255, 200};
                if (warpFactor > 0.8f) {
                    starColor = (Color){255, 255, 255, 255}; 
                }
                Bres_ThickLine(
                    (int)cx, (int)cy,
                    endX, (int)cy,
                    thick, starColor
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
