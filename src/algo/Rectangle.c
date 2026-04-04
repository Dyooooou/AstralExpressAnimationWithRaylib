#include "bresenham.h"
#include <math.h>
#include <stdlib.h>

void RectangleLines(int x, int y, int width, int height, Color color) {
    // Garis Atas
    BresenhamLine(x, y, x + width, y, color);
    // Garis Kanan
    BresenhamLine(x + width, y, x + width, y + height, color);
    // Garis Bawah
    BresenhamLine(x + width, y + height, x, y + height, color);
    // Garis Kiri
    BresenhamLine(x, y + height, x, y, color);
}
