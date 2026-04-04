#include "kereta.h"
#include "bintang.h"
#include "../core/state.h"
#include "../algo/bresenham.h"
#include "../algo/midcircle.h"
#include "../algo/dda.h"
#include <math.h>

#define T(lx, ly) (Vector2){ \
    cx + ((lx * scale) * sx) * cosA - ((ly * scale) * sy) * sinA, \
    cy + ((lx * scale) * sx) * sinA + ((ly * scale) * sy) * cosA  \
}

#define TPOS(lx, ly) (Vector2){ \
    cx + (lx * scale) * cosA - (ly * scale) * sinA, \
    cy + (lx * scale) * sinA + (ly * scale) * cosA  \
}

void drawRoda(float cx, float cy, float r, float angle, Color color) {
    MidcircleFilled((int)cx, (int)cy, (int)r, color);
    MidcircleFilled((int)cx, (int)cy, (int)(r * 0.7f), (Color){40, 40, 45, 255});

    int num_spokes = 8;
    for (int j = 0; j < num_spokes; j++) {
        float a  = angle + j * (2.0f * PI / num_spokes);
        int   px = (int)(cx + (r * 0.8f) * cosf(a));
        int   py = (int)(cy + (r * 0.8f) * sinf(a));
        Bres_ThickLine((int)cx, (int)cy, px, py, 2, color);
    }

    MidcircleFilled((int)cx, (int)cy, (int)(r * 0.3f),  color);
    MidcircleFilled((int)cx, (int)cy, (int)(r * 0.15f), BLACK);
}

void drawKereta(float cx, float cy, float angle, float warpFactor, float rodaTheta) {
    float sx    = 1.0f + warpFactor * 3.0f;
    float sy    = 1.0f - warpFactor * 0.3f;
    float scale = 1.5f;

    float cosA = cosf(angle), sinA = sinf(angle);

    Color merahGelap  = (Color){160,  30,  30, 255};
    Color merahTerang = (Color){200,  50,  50, 255};
    Color hitamBaja   = (Color){ 35,  35,  40, 255};
    Color chrome      = (Color){180, 185, 195, 255};
    Color emas        = (Color){200, 160,  60, 255};
    Color emasTerang  = (Color){240, 200,  80, 255};

    Vector2 b1  = T(-95, -22), b2  = T(-95,  18), b3  = T( 20,  18), b4  = T( 20, -22);
    Vector2 s1  = T(-95, -22), s2  = T(-95, -17), s3  = T( 20, -17), s4  = T( 20, -22);
    Vector2 s5  = T(-95,  13), s6  = T(-95,  18), s7  = T( 20,  18), s8  = T( 20,  13);
    Vector2 k1  = T( 20, -26), k2  = T( 20,  18), k3  = T( 70,  18), k4  = T( 75, -22);
    Vector2 p1  = T( 22, -20), p2  = T( 22,  12), p3  = T( 65,  12), p4  = T( 68, -20);
    Vector2 c1  = T( 52, -22), c2  = T( 52, -38), c3  = T( 62, -38), c4  = T( 62, -22);
    Vector2 ct1 = T( 49, -38), ct2 = T( 49, -43), ct3 = T( 65, -43), ct4 = T( 65, -38);
    Vector2 dome= T( 35, -22);
    Vector2 j1  = T(-75, -20), j2  = T(-75,  -4), j3  = T(-55,  -4), j4  = T(-55, -20);
    Vector2 j5  = T(-45, -20), j6  = T(-45,  -4), j7  = T(-28,  -4), j8  = T(-28, -20);
    Vector2 u1  = T(-95,  18), u2  = T(-95,  24), u3  = T( 75,  24), u4  = T( 75,  18);
    Vector2 cwA = T( 75,  18), cwB = T( 90,  24), cwC = T( 75,  24);
    Vector2 lamp= T( 78,  -5);

    float rodaLX[5] = { 55,  25, -20, -65, -85};
    float rodaLY[5] = { 24,  24,  24,  24,  24};
    float rodaR[5]  = {  8,  14,  14,   8,   8};
    float thetaRoda[5];
    thetaRoda[0] = rodaTheta * (14.0f / 8.0f);
    thetaRoda[1] = rodaTheta;
    thetaRoda[2] = rodaTheta;
    thetaRoda[3] = rodaTheta * (14.0f / 8.0f);
    thetaRoda[4] = rodaTheta * (14.0f / 8.0f);

    if (!showOutline && warpFactor > 0.1f) {
        Vector2 gpos = T(-103, 20);
        float   gr   = 5.0f + warpFactor * 10.0f;
        MidcircleFilled((int)gpos.x, (int)gpos.y, (int)(gr + 4), (Color){0,   150, 255, (unsigned char)(40  * warpFactor)});
        MidcircleFilled((int)gpos.x, (int)gpos.y, (int)gr,        (Color){100, 200, 255, (unsigned char)(150 * warpFactor)});
        MidcircleFilled((int)gpos.x, (int)gpos.y, (int)(gr * 0.4f),(Color){255, 255, 255, (unsigned char)(220 * warpFactor)});
    }

    if (showOutline) {
        BresenhamLine(b1.x, b1.y, b2.x, b2.y, GREEN); BresenhamLine(b2.x, b2.y, b3.x, b3.y, GREEN);
        BresenhamLine(b3.x, b3.y, b4.x, b4.y, GREEN); BresenhamLine(b4.x, b4.y, b1.x, b1.y, GREEN);

        BresenhamLine(s1.x, s1.y, s2.x, s2.y, GREEN); BresenhamLine(s2.x, s2.y, s3.x, s3.y, GREEN);
        BresenhamLine(s3.x, s3.y, s4.x, s4.y, GREEN); BresenhamLine(s4.x, s4.y, s1.x, s1.y, GREEN);
        BresenhamLine(s5.x, s5.y, s6.x, s6.y, GREEN); BresenhamLine(s6.x, s6.y, s7.x, s7.y, GREEN);
        BresenhamLine(s7.x, s7.y, s8.x, s8.y, GREEN); BresenhamLine(s8.x, s8.y, s5.x, s5.y, GREEN);

        BresenhamLine(k1.x, k1.y, k2.x, k2.y, GREEN); BresenhamLine(k2.x, k2.y, k3.x, k3.y, GREEN);
        BresenhamLine(k3.x, k3.y, k4.x, k4.y, GREEN); BresenhamLine(k4.x, k4.y, k1.x, k1.y, GREEN);

        BresenhamLine(p1.x, p1.y, p2.x, p2.y, GREEN); BresenhamLine(p2.x, p2.y, p3.x, p3.y, GREEN);
        BresenhamLine(p3.x, p3.y, p4.x, p4.y, GREEN); BresenhamLine(p4.x, p4.y, p1.x, p1.y, GREEN);

        BresenhamLine(c1.x,  c1.y,  c2.x,  c2.y,  GREEN); BresenhamLine(c2.x,  c2.y,  c3.x,  c3.y,  GREEN);
        BresenhamLine(c3.x,  c3.y,  c4.x,  c4.y,  GREEN); BresenhamLine(c4.x,  c4.y,  c1.x,  c1.y,  GREEN);
        BresenhamLine(ct1.x, ct1.y, ct2.x, ct2.y, GREEN); BresenhamLine(ct2.x, ct2.y, ct3.x, ct3.y, GREEN);
        BresenhamLine(ct3.x, ct3.y, ct4.x, ct4.y, GREEN); BresenhamLine(ct4.x, ct4.y, ct1.x, ct1.y, GREEN);

        Midcircle((int)dome.x, (int)dome.y, (int)(10 * sy), GREEN);

        BresenhamLine(j1.x, j1.y, j2.x, j2.y, GREEN); BresenhamLine(j2.x, j2.y, j3.x, j3.y, GREEN);
        BresenhamLine(j3.x, j3.y, j4.x, j4.y, GREEN); BresenhamLine(j4.x, j4.y, j1.x, j1.y, GREEN);

        BresenhamLine(j5.x, j5.y, j6.x, j6.y, GREEN); BresenhamLine(j6.x, j6.y, j7.x, j7.y, GREEN);
        BresenhamLine(j7.x, j7.y, j8.x, j8.y, GREEN); BresenhamLine(j8.x, j8.y, j5.x, j5.y, GREEN);

        BresenhamLine(u1.x, u1.y, u2.x, u2.y, GREEN); BresenhamLine(u2.x, u2.y, u3.x, u3.y, GREEN);
        BresenhamLine(u3.x, u3.y, u4.x, u4.y, GREEN); BresenhamLine(u4.x, u4.y, u1.x, u1.y, GREEN);

        BresenhamLine(cwA.x, cwA.y, cwB.x, cwB.y, GREEN);
        BresenhamLine(cwB.x, cwB.y, cwC.x, cwC.y, GREEN);
        BresenhamLine(cwC.x, cwC.y, cwA.x, cwA.y, GREEN);

        Midcircle((int)lamp.x, (int)lamp.y, (int)(5 * sy), GREEN);

        Vector2 sv1 = T(70, 16), sv2 = T(70, 23), sv3 = T(80, 23), sv4 = T(80, 16);
        BresenhamLine((int)sv1.x, (int)sv1.y, (int)sv2.x, (int)sv2.y, GREEN);
        BresenhamLine((int)sv2.x, (int)sv2.y, (int)sv3.x, (int)sv3.y, GREEN);
        BresenhamLine((int)sv3.x, (int)sv3.y, (int)sv4.x, (int)sv4.y, GREEN);
        BresenhamLine((int)sv4.x, (int)sv4.y, (int)sv1.x, (int)sv1.y, GREEN);

        for (int i = 0; i < 5; i++) {
            Vector2 wp = TPOS(rodaLX[i], rodaLY[i]);
            Midcircle((int)wp.x, (int)wp.y, (int)(rodaR[i] * sy), GREEN);
        }

        Vector2 pusat1 = TPOS(rodaLX[1], rodaLY[1]);
        Vector2 pusat2 = TPOS(rodaLX[2], rodaLY[2]);
        float   crank  = 10.0f;
        Vector2 pin1   = {pusat1.x + crank * cosf(thetaRoda[1]), pusat1.y + crank * sinf(thetaRoda[1])};
        Vector2 pin2   = {pusat2.x + crank * cosf(thetaRoda[2]), pusat2.y + crank * sinf(thetaRoda[2])};
        BresenhamLine(pin1.x, pin1.y, pin2.x, pin2.y, GREEN);

        float pistonEndX = TPOS(90, rodaLY[1]).x;
        BresenhamLine((int)pin1.x, (int)pin1.y, (int)pistonEndX, (int)pin1.y, GREEN);

        Vector2 cp1 = T(90, 20), cp2 = T(98, 20);
        Vector2 cp3 = T(98, 17), cp4 = T(98, 23);
        BresenhamLine((int)cp1.x, (int)cp1.y, (int)cp2.x, (int)cp2.y, GREEN);
        BresenhamLine((int)cp3.x, (int)cp3.y, (int)cp4.x, (int)cp4.y, GREEN);

        Vector2 cpb1 = T(-95, 20), cpb2 = T(-103, 20);
        BresenhamLine((int)cpb1.x, (int)cpb1.y, (int)cpb2.x, (int)cpb2.y, GREEN);

    } else {

        FillTriangle(b1, b2, b3, merahGelap);  FillTriangle(b1, b3, b4, merahGelap);
        FillTriangle(s1, s2, s3, emas);         FillTriangle(s1, s3, s4, emas);
        FillTriangle(s5, s6, s7, emas);         FillTriangle(s5, s7, s8, emas);

        FillTriangle(k1, k2, k3, merahTerang);  FillTriangle(k1, k3, k4, merahTerang);
        FillTriangle(p1, p2, p3, hitamBaja);    FillTriangle(p1, p3, p4, hitamBaja);
        FillTriangle(c1, c2, c3, hitamBaja);    FillTriangle(c1, c3, c4, hitamBaja);
        FillTriangle(ct1, ct2, ct3, hitamBaja); FillTriangle(ct1, ct3, ct4, hitamBaja);

        MidcircleFilled((int)dome.x, (int)dome.y, (int)(10 * sy), merahTerang);
        Midcircle((int)dome.x, (int)dome.y, (int)(10 * sy), emas);

        FillTriangle(j1, j2, j3, (Color){10, 180, 220, 200}); FillTriangle(j1, j3, j4, (Color){10, 180, 220, 200});
        Bres_ThickLine((int)j1.x, (int)j1.y, (int)j4.x, (int)j4.y, 2, emasTerang);
        Bres_ThickLine((int)j4.x, (int)j4.y, (int)j3.x, (int)j3.y, 2, emasTerang);
        Bres_ThickLine((int)j3.x, (int)j3.y, (int)j2.x, (int)j2.y, 2, emasTerang);
        Bres_ThickLine((int)j2.x, (int)j2.y, (int)j1.x, (int)j1.y, 2, emasTerang);

        FillTriangle(j5, j6, j7, (Color){10, 180, 220, 180}); FillTriangle(j5, j7, j8, (Color){10, 180, 220, 180});
        Bres_ThickLine((int)j5.x, (int)j5.y, (int)j8.x, (int)j8.y, 2, emasTerang);
        Bres_ThickLine((int)j8.x, (int)j8.y, (int)j7.x, (int)j7.y, 2, emasTerang);
        Bres_ThickLine((int)j7.x, (int)j7.y, (int)j6.x, (int)j6.y, 2, emasTerang);
        Bres_ThickLine((int)j6.x, (int)j6.y, (int)j5.x, (int)j5.y, 2, emasTerang);

        FillTriangle(u1, u2, u3, hitamBaja); FillTriangle(u1, u3, u4, hitamBaja);
        Bres_ThickLine((int)u1.x, (int)u1.y, (int)u4.x, (int)u4.y, 2, chrome);

        FillTriangle(cwA, cwB, cwC, (Color){120, 120, 130, 255});
        FillTriangle(T(75, 18), T(75, 24), T(90, 24), (Color){140, 140, 150, 255});
        for (int i = 0; i < 3; i++) {
            float   lx  = 76 + i * 5;
            Vector2 cw1 = T(lx, 18), cw2 = T(lx + 7, 24);
            Bres_ThickLine((int)cw1.x, (int)cw1.y, (int)cw2.x, (int)cw2.y, 1, chrome);
        }

        MidcircleFilled((int)lamp.x, (int)lamp.y, (int)(6 * sy), hitamBaja);
        MidcircleFilled((int)lamp.x, (int)lamp.y, (int)(5 * sy), (Color){255, 240, 180, 230});
        MidcircleFilled((int)lamp.x, (int)lamp.y, (int)(3 * sy), (Color){255, 255, 220, 255});

        for (int i = 0; i < 5; i++) {
            Vector2 wp = TPOS(rodaLX[i], rodaLY[i]);
            drawRoda(wp.x, wp.y, rodaR[i] * sy, thetaRoda[i], chrome);
        }

        Vector2 pusat1    = TPOS(rodaLX[1], rodaLY[1]);
        Vector2 pusat2    = TPOS(rodaLX[2], rodaLY[2]);
        float   crankR    = 10.0f;
        Vector2 pin1      = {pusat1.x + crankR * cosf(thetaRoda[1]), pusat1.y + crankR * sinf(thetaRoda[1])};
        Vector2 pin2      = {pusat2.x + crankR * cosf(thetaRoda[2]), pusat2.y + crankR * sinf(thetaRoda[2])};

        Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pin2.x, (int)pin2.y, 5, (Color){100, 100, 110, 255});
        Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pin2.x, (int)pin2.y, 3, chrome);
        MidcircleFilled((int)pin1.x, (int)pin1.y, 4, chrome); MidcircleFilled((int)pin1.x, (int)pin1.y, 2, hitamBaja);
        MidcircleFilled((int)pin2.x, (int)pin2.y, 4, chrome); MidcircleFilled((int)pin2.x, (int)pin2.y, 2, hitamBaja);

        float pistonEndX = TPOS(90, rodaLY[1]).x;
        Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pistonEndX, (int)pin1.y, 3, (Color){80,  80,  90, 255});
        Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pistonEndX, (int)pin1.y, 2, chrome);

        Vector2 sv1 = T(70, 16), sv2 = T(70, 23), sv3 = T(80, 23), sv4 = T(80, 16);
        FillTriangle(sv1, sv2, sv3, (Color){50, 52, 60, 255});
        FillTriangle(sv1, sv3, sv4, (Color){50, 52, 60, 255});
        Bres_ThickLine((int)sv1.x, (int)sv1.y, (int)sv4.x, (int)sv4.y, 1, chrome);

        Vector2 hr1 = T( 20, -26), hr2 = T(72,  -26);
        Vector2 hr3 = T(-95, -22), hr4 = T(20,  -22);
        Bres_ThickLine((int)hr1.x, (int)hr1.y, (int)hr2.x, (int)hr2.y, 2, emas);
        Bres_ThickLine((int)hr3.x, (int)hr3.y, (int)hr4.x, (int)hr4.y, 1, emas);

        Vector2 cp1 = T(90, 20), cp2 = T(98, 20);
        Vector2 cp3 = T(98, 17), cp4 = T(98, 23);
        Bres_ThickLine((int)cp1.x, (int)cp1.y, (int)cp2.x, (int)cp2.y, 3, chrome);
        Bres_ThickLine((int)cp3.x, (int)cp3.y, (int)cp4.x, (int)cp4.y, 3, chrome);

        Vector2 cpb1 = T(-95, 20), cpb2 = T(-103, 20);
        Bres_ThickLine((int)cpb1.x, (int)cpb1.y, (int)cpb2.x, (int)cpb2.y, 3, chrome);
    }

    #undef T
    #undef TPOS
}