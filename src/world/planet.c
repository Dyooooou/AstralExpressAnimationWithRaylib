#include "planet.h"
#include "bintang.h"
#include "../core/state.h"
#include "../algo/midcircle.h"

Planet planets[NUM_PLANETS];

void randomizePlanets(void) {
    for (int i = 0; i < NUM_PLANETS; i++) {
        planets[i].x      = (float)GetRandomValue(SW / 2, SW + 300);
        planets[i].y      = (float)GetRandomValue(150, SH - 150);
        planets[i].radius = (float)GetRandomValue(40, 120);
        planets[i].speed  = (float)GetRandomValue(3, 10);
        planets[i].color  = (Color){
            (unsigned char)GetRandomValue(30, 200),
            (unsigned char)GetRandomValue(30, 200),
            (unsigned char)GetRandomValue(30, 200),
            255
        };
    }
}

void updatePlanets(float warpFactor, float dt) {
    for (int i = 0; i < NUM_PLANETS; i++) {
        planets[i].x -= planets[i].speed * (1.0f + warpFactor * 8.0f) * dt;
        if (planets[i].x < -300) {
            planets[i].x = SW + 300;
            planets[i].y = (float)GetRandomValue(150, SH - 150);
        }
    }
}

void drawPlanet(float cx, float cy, float r, Color warna) {
    if (showOutline) {
        Midcircle((int)cx, (int)cy, (int)r, GREEN);
        Midcircle((int)(cx - r * 0.3f), (int)(cy - r * 0.3f), (int)(r * 0.55f), GREEN);
        Midcircle((int)(cx + r * 0.2f), (int)(cy + r * 0.2f), (int)(r * 0.7f),  GREEN);
    } else {
        MidcircleFilled((int)cx, (int)cy, (int)r, warna);
        MidcircleFilled(
            (int)(cx - r * 0.3f), (int)(cy - r * 0.3f), (int)(r * 0.55f),
            (Color){warna.r + 40, warna.g + 40, warna.b + 40, 80}
        );
        MidcircleFilled(
            (int)(cx + r * 0.2f), (int)(cy + r * 0.2f), (int)(r * 0.7f),
            (Color){0, 0, 0, 60}
        );
        Midcircle((int)cx, (int)cy, (int)r, (Color){255, 255, 255, 40});
    }
}