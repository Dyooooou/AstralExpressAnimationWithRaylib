#include "bresenham.h"
#include <math.h>
#include <stdlib.h>

void BresenhamLine(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2-x1), dy = abs(y2-y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        DrawPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2*err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void Bres_DashedLine(int x1, int y1, int x2, int y2,
                     int dashLen, int gapLen, Color color) {
    int dx = abs(x2-x1), dy = abs(y2-y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int counter = 0;
    int drawing = 1;
    int current_limit = dashLen;

    while (1) {
        if (drawing) {
            DrawPixel(x1, y1, color);
        }
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2*err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
        
        counter++;
        if (counter >= current_limit) {
            counter = 0;
            drawing = !drawing;
            current_limit = drawing ? dashLen : gapLen;
        }
    }
}

void Bres_ThickLine(int x1, int y1, int x2, int y2, int thick, Color color) {
    float dx = (float)(x2-x1), dy = (float)(y2-y1);
    float len = sqrtf(dx*dx + dy*dy);
    if (len == 0) return;
    float px = -dy/len, py = dx/len;
    int half = thick/2;
    for (int t = -half; t <= half; t++) {
        int ox = (int)roundf(px*t), oy = (int)roundf(py*t);
        BresenhamLine(x1+ox, y1+oy, x2+ox, y2+oy, color);
    }
}

void Bres_DashDotLine(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2-x1), dy = abs(y2-y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int phases[] = {18, 5, 5, 5};
    int drawPh[] = {1, 0, 1, 0};
    int phase = 0;
    int counter = 0;

    while (1) {
        if (drawPh[phase % 4]) {
            DrawPixel(x1, y1, color);
        }
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2*err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
        
        counter++;
        if (counter >= phases[phase % 4]) {
            counter = 0;
            phase++;
        }
    }
}

// ════════════════════════════════════════════════════════════════
//  FUNGSI BARU: Isi segitiga dengan scan-line menggunakan Bresenham
// ════════════════════════════════════════════════════════════════
void FillTriangle(Vector2 a, Vector2 b, Vector2 c, Color color) {
    // Urutkan titik berdasarkan Y (a.y <= b.y <= c.y)
    if (b.y < a.y) { Vector2 tmp = a; a = b; b = tmp; }
    if (c.y < a.y) { Vector2 tmp = a; a = c; c = tmp; }
    if (c.y < b.y) { Vector2 tmp = b; b = c; c = tmp; }

    int y0 = (int)a.y, y1 = (int)b.y, y2 = (int)c.y;
    float x0 = a.x,    x1 = b.x,    x2 = c.x;

    float invTotal = (y2 - y0 != 0) ? 1.0f / (y2 - y0) : 0.0f;
    float invUpper = (y1 - y0 != 0) ? 1.0f / (y1 - y0) : 0.0f;
    float invLower = (y2 - y1 != 0) ? 1.0f / (y2 - y1) : 0.0f;

    // Menggambar bagian atas segitiga (dari a ke b)
    for (int y = y0; y <= y1; y++) {
        float tLong  = (y - y0) * invTotal;
        float tShort = (y - y0) * invUpper;
        float xL = x0 + (x2 - x0) * tLong;
        float xR = x0 + (x1 - x0) * tShort;
        if (xL > xR) { float tmp = xL; xL = xR; xR = tmp; }
        
        // Gunakan BresenhamLine murni untuk mengisi area (scan-line)
        BresenhamLine((int)xL, y, (int)xR, y, color);
    }
    
    // Menggambar bagian bawah segitiga (dari b ke c)
    for (int y = y1; y <= y2; y++) {
        float tLong  = (y - y0) * invTotal;
        float tShort = (y - y1) * invLower;
        float xL = x0 + (x2 - x0) * tLong;
        float xR = x1 + (x2 - x1) * tShort;
        if (xL > xR) { float tmp = xL; xL = xR; xR = tmp; }
        
        // Gunakan BresenhamLine murni untuk mengisi area (scan-line)
        BresenhamLine((int)xL, y, (int)xR, y, color);
    }
}

void FillQuad(Vector2 a, Vector2 b, Vector2 c, Vector2 d, Color color) {
    FillTriangle(a, b, c, color);
    FillTriangle(a, c, d, color);
}

void RectanglesLines(int x, int y, int width, int height, Color color) {
    BresenhamLine(x,         y,          x + width, y,          color);
    BresenhamLine(x + width, y,          x + width, y + height, color);
    BresenhamLine(x + width, y + height, x,         y + height, color);
    BresenhamLine(x,         y + height, x,         y,          color);
}
 
void Algoritma_RectangleFilled(int x, int y, int width, int height, Color color) {
    for (int i = 0; i < height; i++) {
        BresenhamLine(x, y + i, x + width, y + i, color);
    }
}