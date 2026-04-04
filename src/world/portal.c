#include "portal.h"
#include "../core/utils.h"
#include "../algo/bresenham.h"
#include "../algo/midcircle.h"
#include "../algo/dda.h"
#include <math.h>

static void getSkewedHexagonVertices(Vector2 center, float radius, float rotasi, Vector2 *vertices) {
    Vector2 unitPoints[6] = {
        { 0.2f, -1.0f},
        { 0.9f, -0.4f},
        { 0.7f,  0.8f},
        {-0.1f,  0.9f},
        {-0.9f,  0.3f},
        {-0.6f, -0.7f}
    };

    float cosA = cosf(rotasi), sinA = sinf(rotasi);
    for (int i = 0; i < 6; i++) {
        float vx = unitPoints[i].x * radius;
        float vy = unitPoints[i].y * radius;
        vertices[i].x = center.x + vx * cosA - vy * sinA;
        vertices[i].y = center.y + vx * sinA + vy * cosA;
    }
}

void drawPortal(float cx, float cy, float r, float rotasi, float openFactor) {
    if (openFactor <= 0.01f) return;

    Color idleBase = (Color){0,   180, 255, 255};
    Color warpBase = (Color){220,  40, 200, 255};

    Color c_int  = MyColorLerp((Color){5, 10, 25, 240}, (Color){40, 5, 50, 240},    openFactor);
    Color c_warp = MyColorLerp(idleBase, warpBase,                                   openFactor);

    Vector2 pnts[6];

    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) + 20, rotasi, pnts);
    for (int i = 1; i <= 4; i++)
        FillTriangle(pnts[0], pnts[i], pnts[i + 1], (Color){c_warp.r, c_warp.g, c_warp.b, 25});

    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) + 5, rotasi, pnts);
    for (int i = 1; i <= 4; i++)
        FillTriangle(pnts[0], pnts[i], pnts[i + 1], (Color){c_warp.r, c_warp.g, c_warp.b, 50});

    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) - 6, rotasi, pnts);
    for (int i = 1; i <= 4; i++)
        FillTriangle(pnts[0], pnts[i], pnts[i + 1], c_int);

    int thick = 3;

    getSkewedHexagonVertices((Vector2){cx, cy}, r * openFactor, rotasi, pnts);
    Color c_line = MyColorLerp((Color){0, 220, 255, 200}, (Color){255, 150, 220, 200}, openFactor);
    for (int i = 0; i < 6; i++)
        Bres_ThickLine(
            (int)pnts[i].x, (int)pnts[i].y,
            (int)pnts[(i + 1) % 6].x, (int)pnts[(i + 1) % 6].y,
            thick, c_line
        );

    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) - 8, rotasi + 0.15f, pnts);
    Color c_line_in = MyColorLerp((Color){150, 240, 255, 150}, (Color){255, 200, 240, 150}, openFactor);
    for (int i = 0; i < 6; i++)
        Bres_ThickLine(
            (int)pnts[i].x, (int)pnts[i].y,
            (int)pnts[(i + 1) % 6].x, (int)pnts[(i + 1) % 6].y,
            2, c_line_in
        );

    MidcircleFilled(
        (int)cx, (int)cy, (int)((r * openFactor) * 0.15f),
        (Color){c_warp.r, c_warp.g, c_warp.b, (unsigned char)(100 * openFactor)}
    );
    MidcircleFilled(
        (int)cx, (int)cy, (int)((r * openFactor) * 0.05f),
        (Color){255, 255, 255, (unsigned char)(200 * openFactor)}
    );
}