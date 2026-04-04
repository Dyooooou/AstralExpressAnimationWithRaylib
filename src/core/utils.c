#include "utils.h"

float lerpF(float a, float b, float t) {
    return a + (b - a) * t;
}

float clamp01(float t) {
    return t < 0 ? 0 : (t > 1 ? 1 : t);
}

Color MyColorLerp(Color c1, Color c2, float t) {
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;
    return (Color){
        (unsigned char)(c1.r + t * (c2.r - c1.r)),
        (unsigned char)(c1.g + t * (c2.g - c1.g)),
        (unsigned char)(c1.b + t * (c2.b - c1.b)),
        (unsigned char)(c1.a + t * (c2.a - c1.a))
    };
}