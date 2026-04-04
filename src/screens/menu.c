#include "menu.h"
#include "../core/state.h"
#include "../core/utils.h"
#include "../algo/bresenham.h"
#include "../algo/midcircle.h"
#include "../ui/primitives.h"
#include "../world/kereta.h"
#include "../world/portal.h"
#include <math.h>

int   menuOption      = 0;
int   mekanismeSlide  = 0;
float mekanismeTimer  = 0.0f;

void drawMainMenu(void) {
    DrawText("ASTRAL EXPRESS SIMULATOR", SW / 2 - 250, 200, 40, WHITE);
    DrawText("Gunakan Panah Atas/Bawah dan ENTER", SW / 2 - 180, 260, 20, GRAY);

    const char *options[] = {
        "1. PLAY SIMULATION",
        "2. GUIDE / KONTROL",
        "3. MEKANISME ANIMASI",
        "4. EXIT"
    };

    for (int i = 0; i < 4; i++) {
        Color textColor = (i == menuOption) ? YELLOW : WHITE;
        if (i == menuOption)
            RectanglesLines(SW / 2 - 160, 400 + i * 60 - 10, 320, 40, YELLOW);
        DrawText(options[i], SW / 2 - 140, 400 + i * 60, 20, textColor);
    }
}

void drawGuide(void) {
    DrawText("GUIDE / KONTROL SIMULASI", 100, 100, 40, WHITE);
    DrawText("- [SPACE] : Memulai Proses Warp Jump (Charge -> Warp)",          100, 200, 20, WHITE);
    DrawText("- [R]     : Mereset simulasi ke titik awal",                      100, 250, 20, WHITE);
    DrawText("- [T]     : Toggle Mode Outline (Bounding Box & Primitif)",       100, 300, 20, WHITE);
    DrawText("- [H]     : Sembunyikan/Tampilkan HUD Informasi Bawah",           100, 350, 20, WHITE);
    DrawText("Tekan [ENTER] atau [BACKSPACE] untuk kembali ke Menu Utama",      100, SH - 100, 20, YELLOW);
}

void drawMekanisme(float timer) {
    DrawText("BEDAH MEKANISME & KINEMATIKA", 60, 40, 35, WHITE);
    DrawText("Gunakan [PANAH KIRI / KANAN] untuk ganti slide | [ENTER] kembali", 60, SH - 50, 20, YELLOW);

    if (mekanismeSlide == 0) {
        DrawText("1. DEFORMASI WARP — Skala Linier & Bintang Streak", 60, 90, 26, (Color){80, 220, 120, 255});

        float wf = (sinf(timer * 1.8f) + 1.0f) * 0.5f;

        DrawText("Kereta:", 60, 135, 18, WHITE);
        float kx = 500.0f, ky = 340.0f;
        drawKereta(kx, ky, 0.0f, wf, timer * -3.0f);

        float baseHalf    = 95.0f * 1.5f;
        float stretchHalf = baseHalf * (1.0f + wf * 3.0f);
        int   arrY        = (int)ky + 80;
        Bres_ThickLine((int)(kx - stretchHalf), arrY, (int)(kx + stretchHalf * 0.22f), arrY, 2, (Color){255, 200, 50, 200});
        MidcircleFilled((int)(kx - stretchHalf),       arrY, 4, (Color){255, 200, 50, 255});
        MidcircleFilled((int)(kx + stretchHalf * 0.22f), arrY, 4, (Color){255, 200, 50, 255});
        DrawText(TextFormat("sx = 1 + %.2f*3 = %.2f", wf, 1.0f + wf * 3.0f),
                 (int)(kx - stretchHalf), arrY + 10, 16, (Color){255, 200, 50, 255});

        DrawText("Bintang streak:", 1050, 135, 18, WHITE);
        for (int i = 0; i < 12; i++) {
            float bx  = 1100.0f + (i % 4) * 190.0f;
            float by  = 220.0f  + (i / 4) * 130.0f;
            float spd = 25.0f   + i * 7.0f;
            float sz  = 1.5f    + (i % 3) * 0.8f;
            if (wf > 0.2f) {
                float streak = wf * spd * 0.3f;
                int   thick  = (int)(sz * 0.7f);
                if (thick < 1) thick = 1;
                Bres_ThickLine((int)bx, (int)by, (int)(bx + streak), (int)by, thick, (Color){200, 220, 255, 200});
            } else {
                MidcircleFilled((int)bx, (int)by, (int)sz, (Color){200, 220, 255, 180});
            }
        }

        Algoritma_RectangleFilled(50, SH - 300, 700, 220, (Color){5, 12, 30, 210});
        Bres_ThickLine(50, SH - 300, 750, SH - 300, 1, (Color){80, 220, 120, 150});
        DrawText("RUMUS DEFORMASI KERETA:",                                               70, SH - 285, 18, (Color){80, 220, 120, 255});
        DrawText("sx  = 1.0 + warpFactor * 3.0   (skala sumbu X)",                       70, SH - 255, 16, WHITE);
        DrawText("sy  = 1.0 - warpFactor * 0.3   (skala sumbu Y, mengempis)",            70, SH - 230, 16, WHITE);
        DrawText("X'  = cx + (lx * scale * sx) * cos(a) - (ly * scale * sy) * sin(a)",  70, SH - 205, 16, (Color){180, 220, 255, 255});
        DrawText("Y'  = cy + (lx * scale * sx) * sin(a) + (ly * scale * sy) * cos(a)",  70, SH - 180, 16, (Color){180, 220, 255, 255});
        DrawText(TextFormat("Live  >>  warpFactor = %.3f   sx = %.3f   sy = %.3f",
                            wf, 1.0f + wf * 3.0f, 1.0f - wf * 0.3f),                    70, SH - 145, 16, (Color){255, 220, 80, 255});

        Algoritma_RectangleFilled(SW - 760, SH - 300, 700, 220, (Color){5, 12, 30, 210});
        Bres_ThickLine(SW - 760, SH - 300, SW - 60, SH - 300, 1, (Color){80, 220, 120, 150});
        DrawText("RUMUS BINTANG STREAK:",                                        SW - 740, SH - 285, 18, (Color){80, 220, 120, 255});
        DrawText("speed_eff = speed * (1 + warpFactor * 10)",                   SW - 740, SH - 255, 16, WHITE);
        DrawText("streak    = warpFactor * speed * 0.3",                        SW - 740, SH - 230, 16, WHITE);
        DrawText("Gambar: ThickLine(x, y,  x + streak, y)",                     SW - 740, SH - 205, 16, (Color){180, 220, 255, 255});
        DrawText(TextFormat("Live  >>  streak contoh = %.1f px", wf * 40.0f * 0.3f),
                                                                                 SW - 740, SH - 145, 16, (Color){255, 220, 80, 255});

    } else if (mekanismeSlide == 1) {
        DrawText("2. KINEMATIKA RODA — Connecting Rod & Piston", 60, 90, 26, (Color){80, 180, 255, 255});
        float theta = timer * -3.5f;

        DrawText("Kereta dengan roda berputar (aset asli):", 60, 135, 18, WHITE);
        float kx = SW / 2.0f, ky = 340.0f;
        drawKereta(kx, ky, 0.0f, 0.0f, theta);

        float rodaLX1 = 25.0f, rodaLX2 = -20.0f, rodaLY = 24.0f, scale = 1.5f;
        float w1x = kx + rodaLX1 * scale, w1y = ky + rodaLY * scale;
        float w2x = kx + rodaLX2 * scale, w2y = ky + rodaLY * scale;
        float crankR = 10.0f;
        float pin1x  = w1x + crankR * cosf(theta), pin1y = w1y + crankR * sinf(theta);
        float pin2x  = w2x + crankR * cosf(theta), pin2y = w2y + crankR * sinf(theta);

        Midcircle((int)w1x, (int)w1y, 25, (Color){255, 200, 0, 180});
        Midcircle((int)w2x, (int)w2y, 25, (Color){255, 200, 0, 180});
        Bres_ThickLine((int)w1x, (int)w1y, (int)pin1x, (int)pin1y, 2, (Color){255, 100, 100, 220});
        Bres_ThickLine((int)w2x, (int)w2y, (int)pin2x, (int)pin2y, 2, (Color){255, 100, 100, 220});
        DrawText("A",  (int)pin1x + 6, (int)pin1y - 14, 16, (Color){255, 100, 100, 255});
        DrawText("B",  (int)pin2x + 6, (int)pin2y - 14, 16, (Color){255, 100, 100, 255});
        DrawText("O1", (int)w1x   + 6, (int)w1y   - 22, 16, (Color){255, 200,   0, 255});
        DrawText("O2", (int)w2x   + 6, (int)w2y   - 22, 16, (Color){255, 200,   0, 255});

        int cx2 = 350, cy2 = 700, R = 80, cr = 30;
        DrawText("Detail: Crank-Slider (Roda + Rod + Piston)", 60, 590, 18, WHITE);
        MidcircleFilled(cx2, cy2, R, (Color){180, 185, 195, 255});
        MidcircleFilled(cx2, cy2, (int)(R * 0.7f), (Color){40, 40, 45, 255});
        for (int j = 0; j < 8; j++) {
            float a  = theta + j * (2.0f * PI / 8.0f);
            int   px = cx2 + (int)((R * 0.8f) * cosf(a));
            int   py = cy2 + (int)((R * 0.8f) * sinf(a));
            Bres_ThickLine(cx2, cy2, px, py, 2, (Color){180, 185, 195, 255});
        }
        MidcircleFilled(cx2, cy2, (int)(R * 0.3f),  (Color){180, 185, 195, 255});
        MidcircleFilled(cx2, cy2, (int)(R * 0.15f), (Color){0, 0, 0, 255});

        int pinX = cx2 + (int)(cr * cosf(theta));
        int pinY = cy2 + (int)(cr * sinf(theta));
        MidcircleFilled(pinX, pinY, 8, (Color){255, 100, 100, 255});

        int pistonX = cx2 + 200;
        Bres_ThickLine(cx2, cy2, pinX, pinY, 3, (Color){255, 100, 100, 200});
        Bres_ThickLine(pinX, pinY, pistonX, pinY, 3, (Color){180, 185, 195, 200});

        Algoritma_RectangleFilled(pistonX, pinY - 18, 40, 36, (Color){80, 85, 95, 255});
        Bres_ThickLine(pistonX,      pinY - 18, pistonX + 40, pinY - 18, 2, (Color){180, 185, 195, 200});
        Bres_ThickLine(pistonX + 40, pinY - 18, pistonX + 40, pinY + 18, 2, (Color){180, 185, 195, 200});
        Bres_ThickLine(pistonX + 40, pinY + 18, pistonX,      pinY + 18, 2, (Color){180, 185, 195, 200});
        Bres_ThickLine(pistonX,      pinY - 18, pistonX + 80, pinY - 18, 1, (Color){100, 100, 110, 150});
        Bres_ThickLine(pistonX,      pinY + 18, pistonX + 80, pinY + 18, 1, (Color){100, 100, 110, 150});

        DrawText(TextFormat("Pin X = %d + %d*cos(%.1f) = %d", cx2, cr, theta, pinX),  480, 640, 16, (Color){255, 100, 100, 255});
        DrawText(TextFormat("Pin Y = %d + %d*sin(%.1f) = %d", cx2, cr, theta, pinY),  480, 665, 16, (Color){255, 100, 100, 255});
        DrawText("Piston X = Pin X + panjang_rod",                                     480, 695, 16, (Color){180, 220, 255, 255});

        Algoritma_RectangleFilled(SW - 780, SH - 310, 720, 235, (Color){5, 12, 30, 210});
        Bres_ThickLine(SW - 780, SH - 310, SW - 60, SH - 310, 1, (Color){80, 180, 255, 150});
        DrawText("RUMUS KINEMATIKA RODA:",                                                   SW - 760, SH - 292, 18, (Color){80, 180, 255, 255});
        DrawText("Pin  : Px = Cx + r * cos(theta)",                                         SW - 760, SH - 265, 16, WHITE);
        DrawText("         Py = Cy + r * sin(theta)",                                       SW - 760, SH - 242, 16, WHITE);
        DrawText("Rod  : Sambungkan (Px1,Py1) -> (Px2,Py2) via Bresenham",                 SW - 760, SH - 218, 16, WHITE);
        DrawText("theta_i  : berbeda per roda (r_besar / r_kecil) * theta_utama",          SW - 760, SH - 193, 16, WHITE);
        DrawText("omega    : 2.0 + warpFactor * 12.0  [rad/s]",                            SW - 760, SH - 168, 16, WHITE);
        DrawText("theta   += -omega * dt  (berlawanan jarum jam)",                          SW - 760, SH - 143, 16, WHITE);
        DrawText(TextFormat("Live  >>  theta = %.2f rad   omega = %.2f rad/s", theta, 3.5f),
                                                                                             SW - 760, SH - 108, 16, (Color){255, 220, 80, 255});

    } else if (mekanismeSlide == 2) {
        DrawText("3. PORTAL WARP — Segienam Skewed, Rotasi & Lerp Warna", 60, 90, 26, (Color){220, 80, 220, 255});

        float openL = 0.45f + 0.2f * sinf(timer * 1.2f), rotL = timer * 1.5f;
        drawPortal(420.0f, SH / 2.0f - 30.0f, 140.0f, rotL, openL);

        float openR = 0.75f + 0.2f * sinf(timer * 0.9f + 1.0f), rotR = -timer * 1.5f;
        drawPortal(1500.0f, SH / 2.0f - 30.0f, 140.0f, rotR, openR);

        DrawText(TextFormat("openFactor = %.2f", openL), 290, SH / 2 + 130, 16, (Color){0, 210, 255, 230});
        DrawText("(Idle / Biru)",                         290, SH / 2 + 150, 16, (Color){0, 180, 255, 180});
        DrawText(TextFormat("openFactor = %.2f", openR), 1370, SH / 2 + 130, 16, (Color){220, 100, 220, 230});
        DrawText("(Warp / Pink)",                         1370, SH / 2 + 150, 16, (Color){200,  80, 200, 180});

        int   hcx     = SW / 2, hcy = 430;
        float rotDiag = timer * 0.6f;
        DrawText("Titik unit segienam skewed (diputar & diskalakan):", hcx - 280, 140, 17, WHITE);

        float openDiag = 0.6f + 0.15f * sinf(timer * 2.0f);
        drawPortal((float)hcx, (float)hcy, 100.0f, rotDiag, openDiag);

        Vector2 unitPts[6] = {{0.2f,-1.0f},{0.9f,-0.4f},{0.7f,0.8f},{-0.1f,0.9f},{-0.9f,0.3f},{-0.6f,-0.7f}};
        float rad  = 100.0f * openDiag;
        float cosD = cosf(rotDiag), sinD = sinf(rotDiag);
        Color dotCol = (Color){255, 230, 80, 255};
        for (int i = 0; i < 6; i++) {
            float vx = unitPts[i].x * rad, vy = unitPts[i].y * rad;
            int   px = hcx + (int)(vx * cosD - vy * sinD);
            int   py = hcy + (int)(vx * sinD + vy * cosD);
            MidcircleFilled(px, py, 6, dotCol);
            DrawText(TextFormat("P%d", i + 1), px + 8, py - 8, 14, dotCol);
        }

        Algoritma_RectangleFilled(40, SH - 310, 710, 240, (Color){10, 5, 25, 215});
        Bres_ThickLine(40, SH - 310, 750, SH - 310, 1, (Color){220, 80, 220, 150});
        DrawText("RUMUS PORTAL:",                                              60, SH - 292, 18, (Color){220, 80, 220, 255});
        DrawText("vx = unitPts[i].x * (r * openFactor)",                     60, SH - 265, 15, WHITE);
        DrawText("vy = unitPts[i].y * (r * openFactor)",                     60, SH - 243, 15, WHITE);
        DrawText("Px = cx + vx*cos(rot) - vy*sin(rot)",                     60, SH - 220, 15, (Color){180, 180, 255, 255});
        DrawText("Py = cy + vx*sin(rot) + vy*cos(rot)",                     60, SH - 197, 15, (Color){180, 180, 255, 255});
        DrawText("rot += dt * (1 + warpFactor * 3)  [semakin cepat saat warp]", 60, SH - 173, 15, WHITE);
        DrawText(TextFormat("Live  >>  rot = %.2f rad   openL = %.2f   openR = %.2f",
                            rotDiag, openL, openR),                          60, SH - 135, 15, (Color){255, 220, 80, 255});

        Algoritma_RectangleFilled(SW - 760, SH - 310, 700, 240, (Color){10, 5, 25, 215});
        Bres_ThickLine(SW - 760, SH - 310, SW - 60, SH - 310, 1, (Color){220, 80, 220, 150});
        DrawText("LERP WARNA PORTAL:",                                       SW - 740, SH - 292, 18, (Color){220, 80, 220, 255});
        DrawText("c_warp = Lerp(biru, pink, openFactor)",                   SW - 740, SH - 265, 15, WHITE);
        DrawText("c_int  = Lerp(hitam_gelap, ungu, openFactor)",            SW - 740, SH - 243, 15, WHITE);
        DrawText("Lerp(a, b, t) = a + t * (b - a)",                        SW - 740, SH - 220, 15, (Color){180, 180, 255, 255});
        DrawText("Transparansi lapis 1 : alpha = 25",                       SW - 740, SH - 197, 15, WHITE);
        DrawText("Transparansi lapis 2 : alpha = 50",                       SW - 740, SH - 175, 15, WHITE);

        Color swL = MyColorLerp((Color){0, 180, 255, 255}, (Color){220, 40, 200, 255}, openL);
        Color swR = MyColorLerp((Color){0, 180, 255, 255}, (Color){220, 40, 200, 255}, openR);
        Algoritma_RectangleFilled(SW - 740, SH - 148, 120, 28, swL);
        DrawText(TextFormat("c kiri  (%.2f)", openL), SW - 610, SH - 143, 14, WHITE);
        Algoritma_RectangleFilled(SW - 740, SH - 112, 120, 28, swR);
        DrawText(TextFormat("c kanan (%.2f)", openR), SW - 610, SH - 107, 14, WHITE);
    } else if (mekanismeSlide == 2) {
        DrawText("3. PORTAL WARP — Segienam Skewed, Rotasi & Lerp Warna",
                 60, 90, 26, (Color){220, 80, 220, 255});
 
        // Portal kiri: openFactor kecil (idle biru)
        float openL = 0.45f + 0.2f * sinf(timer * 1.2f);
        float rotL  = timer * 1.5f;
        drawPortal(420.0f, SH/2.0f - 30.0f, 140.0f, rotL, openL);
 
        // Portal kanan: openFactor besar (warp merah muda)
        float openR = 0.75f + 0.2f * sinf(timer * 0.9f + 1.0f);
        float rotR  = -timer * 1.5f;
        drawPortal(1500.0f, SH/2.0f - 30.0f, 140.0f, rotR, openR);
 
        // Label portal
        DrawText(TextFormat("openFactor = %.2f", openL), 290, SH/2 + 130, 16, (Color){0,210,255,230});
        DrawText("(Idle / Biru)", 290, SH/2 + 150, 16, (Color){0,180,255,180});
        DrawText(TextFormat("openFactor = %.2f", openR), 1370, SH/2 + 130, 16, (Color){220,100,220,230});
        DrawText("(Warp / Pink)", 1370, SH/2 + 150, 16, (Color){200,80,200,180});
 
        // ── Diagram titik segienam tengah layar ──────────────────
        int hcx = SW/2, hcy = 430, hr = 110;
        float rotDiag = timer * 0.6f;
        DrawText("Titik unit segienam skewed (diputar & diskalakan):", hcx - 280, 140, 17, WHITE);
 
        // Gambar portal sesungguhnya di tengah sebagai referensi
        float openDiag = 0.6f + 0.15f*sinf(timer*2.0f);
        drawPortal((float)hcx, (float)hcy, 100.0f, rotDiag, openDiag);
 
        // Overlay titik dan nomor
        Vector2 unitPts[6] = {
            {0.2f,-1.0f},{0.9f,-0.4f},{0.7f,0.8f},
            {-0.1f,0.9f},{-0.9f,0.3f},{-0.6f,-0.7f}
        };
        float rad = 100.0f * openDiag;
        float cosD = cosf(rotDiag), sinD = sinf(rotDiag);
        Color dotCol = (Color){255,230,80,255};
        for (int i = 0; i < 6; i++) {
            float vx = unitPts[i].x * rad;
            float vy = unitPts[i].y * rad;
            int px = hcx + (int)(vx*cosD - vy*sinD);
            int py = hcy + (int)(vx*sinD + vy*cosD);
            MidcircleFilled(px, py, 6, dotCol);
            DrawText(TextFormat("P%d", i+1), px + 8, py - 8, 14, dotCol);
        }
 
        // ── Box Rumus kiri bawah ──────────────────────────────────
        DrawRectangle(40, SH-310, 710, 240, (Color){10,5,25,215});
        Bres_ThickLine(40, SH-310, 750, SH-310, 1, (Color){220,80,220,150});
        DrawText("RUMUS PORTAL:", 60, SH-292, 18, (Color){220,80,220,255});
        DrawText("vx = unitPts[i].x * (r * openFactor)", 60, SH-265, 15, WHITE);
        DrawText("vy = unitPts[i].y * (r * openFactor)", 60, SH-243, 15, WHITE);
        DrawText("Px = cx + vx*cos(rot) - vy*sin(rot)", 60, SH-220, 15, (Color){180,180,255,255});
        DrawText("Py = cy + vx*sin(rot) + vy*cos(rot)", 60, SH-197, 15, (Color){180,180,255,255});
        DrawText("rot += dt * (1 + warpFactor * 3)  [semakin cepat saat warp]", 60, SH-173, 15, WHITE);
        DrawText(TextFormat("Live  >>  rot = %.2f rad   openL = %.2f   openR = %.2f",
                 rotDiag, openL, openR), 60, SH-135, 15, (Color){255,220,80,255});
 
        // ── Box Rumus kanan bawah ─────────────────────────────────
        DrawRectangle(SW-760, SH-310, 700, 240, (Color){10,5,25,215});
        Bres_ThickLine(SW-760, SH-310, SW-60, SH-310, 1, (Color){220,80,220,150});
        DrawText("LERP WARNA PORTAL:", SW-740, SH-292, 18, (Color){220,80,220,255});
        DrawText("c_warp = Lerp(biru, pink, openFactor)", SW-740, SH-265, 15, WHITE);
        DrawText("c_int  = Lerp(hitam_gelap, ungu, openFactor)", SW-740, SH-243, 15, WHITE);
        DrawText("Lerp(a, b, t) = a + t * (b - a)", SW-740, SH-220, 15, (Color){180,180,255,255});
        DrawText("Transparansi lapis 1 : alpha = 25", SW-740, SH-197, 15, WHITE);
        DrawText("Transparansi lapis 2 : alpha = 50", SW-740, SH-175, 15, WHITE);
 
        // Live color swatch
        Color swL = MyColorLerp((Color){0,180,255,255}, (Color){220,40,200,255}, openL);
        Color swR = MyColorLerp((Color){0,180,255,255}, (Color){220,40,200,255}, openR);
        DrawRectangle(SW-740, SH-148, 120, 28, swL);
        DrawText(TextFormat("c kiri  (%.2f)", openL), SW-610, SH-143, 14, WHITE);
        DrawRectangle(SW-740, SH-112, 120, 28, swR);
        DrawText(TextFormat("c kanan (%.2f)", openR), SW-610, SH-107, 14, WHITE);
    }
 
    // ══════════════════════════════════════════════════════════════
    // SLIDE 3: SCISSOR CLIPPING — Kereta Masuk/Keluar Portal
    // ══════════════════════════════════════════════════════════════
    else if (mekanismeSlide == 3) {
        DrawText("4. SCISSOR CLIPPING — Kereta Menembus Portal",
                 60, 90, 26, (Color){255, 160, 60, 255});
 
        // ── Konstanta posisi demo ─────────────────────────────────
        // Animasi: kereta bergerak dari kiri ke kanan melewati portal
        // periodenya 5 detik, kereta bergerak dari x=200 sampai x=1700
        float period   = 6.0f;
        float tNorm    = fmodf(timer, period) / period;      // 0..1 loop
        float portalDX = 1380.0f;   // posisi x portal kanan (tetap)
        float leftPX   = 340.0f;    // posisi x portal kiri  (tetap)
        // Kereta bergerak dari jauh kiri (muncul dari portal kiri) ke kanan
        float trainX   = leftPX - 500.0f + tNorm * (portalDX + 900.0f);
        float trainY   = (float)(SH / 2) - 80.0f;
        float portalY2 = trainY;
        float portalRt = timer * 1.5f;
 
        // Hitung kepala & ekor kereta (approx world-space, scale=1.5, wf=0)
        // badan lokal: x dari -95 sampai +98 (cowcatcher), scale 1.5
        float ekorX  = trainX + (-95.0f) * 1.5f;   // ujung kiri kereta
        float kepalX = trainX + ( 98.0f) * 1.5f;   // ujung kanan kereta
 
        // Tentukan fase clipping sesuai posisi kereta
        // Fase A: kereta belum menyentuh portal kiri  → gambar bebas
        // Fase B: ekor melewati portal kiri, kepala belum di portal kanan
        //         → clip kiri: scissor mulai dari leftPX
        // Fase C: kepala sudah melewati portal kanan
        //         → clip kanan: scissor dari leftPX s/d portalDX
        // (Fase C sini = WARP_LOOP dalam simulasi asli)
        int clipMode = 0; // 0=bebas, 1=clip kiri, 2=clip dua sisi
        if (kepalX > portalDX) clipMode = 2;
        else if (ekorX < leftPX && kepalX < portalDX) clipMode = 1;
 
        // ── Gambar portal kiri & kanan (aset asli) ───────────────
        drawPortal(leftPX,   portalY2, 110.0f,  portalRt, 1.0f);
        drawPortal(portalDX, portalY2, 110.0f, -portalRt, 1.0f);
 
        // ── Gambar kereta dengan scissor yang sesuai ─────────────
        if (clipMode == 0) {
            drawKereta(trainX, trainY, 0.0f, 0.0f, timer * -3.0f);
        } else if (clipMode == 1) {
            // Ekor sudah di dalam portal kiri → potong sisi kiri
            int csx = (int)leftPX;
            int csw = SW - csx;
            BeginScissorMode(csx, 0, csw, SH);
            drawKereta(trainX, trainY, 0.0f, 0.0f, timer * -3.0f);
            EndScissorMode();
        } else {
            // Kepala menembus portal kanan, ekor masih di kiri
            // → potong kedua sisi: hanya gambar area antara dua portal
            int csx = (int)leftPX;
            int csw = (int)(portalDX - leftPX);
            BeginScissorMode(csx, 0, csw, SH);
            drawKereta(trainX, trainY, 0.0f, 0.0f, timer * -3.0f);
            EndScissorMode();
        }
 
        // ── Overlay: garis vertikal batas scissor ────────────────
        // Garis portal kiri (merah putus-putus)
        for (int yy = 0; yy < SH; yy += 18)
            Bres_ThickLine((int)leftPX, yy, (int)leftPX, yy + 9, 1,
                           (Color){255, 80, 80, 180});
        // Garis portal kanan (kuning putus-putus)
        for (int yy = 0; yy < SH; yy += 18)
            Bres_ThickLine((int)portalDX, yy, (int)portalDX, yy + 9, 1,
                           (Color){255, 220, 50, 180});
 
        // Label batas
        DrawText("Portal Kiri", (int)leftPX - 48, (int)trainY - 140, 15,
                 (Color){255, 100, 100, 230});
        DrawText(TextFormat("x = %.0f", leftPX), (int)leftPX - 38, (int)trainY - 120, 14,
                 (Color){255, 100, 100, 200});
        DrawText("Portal Kanan", (int)portalDX - 52, (int)trainY - 140, 15,
                 (Color){255, 220, 50, 230});
        DrawText(TextFormat("x = %.0f", portalDX), (int)portalDX - 42, (int)trainY - 120, 14,
                 (Color){255, 220, 50, 200});
 
        // Label scissor region aktif (di atas area yang di-render)
        int midX = (int)((leftPX + portalDX) / 2.0f);
        if (clipMode == 1) {
            DrawText("[ SCISSOR AKTIF: x >= leftPortalX ]",
                     midX - 170, (int)trainY - 165, 16, (Color){255,160,60,255});
        } else if (clipMode == 2) {
            DrawText("[ SCISSOR AKTIF: leftPortalX <= x < portalX ]",
                     midX - 230, (int)trainY - 165, 16, (Color){255,160,60,255});
        } else {
            DrawText("[ Tidak ada clipping — kereta di luar portal ]",
                     midX - 210, (int)trainY - 165, 16, (Color){120,220,120,255});
        }
 
        // Anotasi kepala & ekor kereta
        DrawText("Ekor", (int)ekorX - 14, (int)trainY - 50, 14, (Color){200,200,255,200});
        DrawText("Kepala", (int)kepalX - 20, (int)trainY - 50, 14, (Color){200,200,255,200});
        Bres_ThickLine((int)ekorX,  (int)trainY - 42, (int)ekorX,  (int)trainY - 28, 1,
                       (Color){200,200,255,180});
        Bres_ThickLine((int)kepalX, (int)trainY - 42, (int)kepalX, (int)trainY - 28, 1,
                       (Color){200,200,255,180});
 
        // ── Nilai live ────────────────────────────────────────────
        DrawText(TextFormat("ekorX  = trainX + (-95 * 1.5) = %.0f", ekorX),
                 60, (int)trainY + 110, 15, (Color){200,200,255,220});
        DrawText(TextFormat("kepalX = trainX + ( 98 * 1.5) = %.0f", kepalX),
                 60, (int)trainY + 132, 15, (Color){200,200,255,220});
 
        // ── Panel Rumus Kiri Bawah ────────────────────────────────
        DrawRectangle(40, SH - 320, 740, 258, (Color){8, 5, 20, 218});
        Bres_ThickLine(40, SH-320, 780, SH-320, 1, (Color){255,160,60,150});
        DrawText("RUMUS SCISSOR CLIPPING:", 60, SH-302, 18, (Color){255,160,60,255});
        DrawText("// Fase WARP: kepala menyentuh portal kanan", 60, SH-276, 14, (Color){120,200,120,220});
        DrawText("if (kepalX > portalX)", 60, SH-256, 15, WHITE);
        DrawText("    BeginScissorMode(0, 0, portalX, SH);", 60, SH-236, 15, (Color){180,220,255,255});
        DrawText("// Fase WARP_LOOP: diapit 2 portal", 60, SH-210, 14, (Color){120,200,120,220});
        DrawText("clipStartX = leftPortalX;", 60, SH-190, 15, WHITE);
        DrawText("clipWidth  = portalX - leftPortalX;", 60, SH-170, 15, WHITE);
        DrawText("BeginScissorMode(clipStartX, 0, clipWidth, SH);", 60, SH-150, 15,
                 (Color){180,220,255,255});
        DrawText("// Fase ARRIVAL: keluar portal kiri", 60, SH-124, 14, (Color){120,200,120,220});
        DrawText("clipWidth  = SW - leftPortalX;", 60, SH-104, 15, WHITE);
        DrawText("BeginScissorMode(leftPortalX, 0, clipWidth, SH);", 60, SH-84, 15,
                 (Color){180,220,255,255});
 
        // ── Panel Konsep Kanan Bawah ──────────────────────────────
        DrawRectangle(SW - 760, SH - 320, 700, 258, (Color){8, 5, 20, 218});
        Bres_ThickLine(SW-760, SH-320, SW-60, SH-320, 1, (Color){255,160,60,150});
        DrawText("KONSEP SCISSOR (GPU Rectangle Clip):", SW-740, SH-302, 18,
                 (Color){255,160,60,255});
        DrawText("Scissor mendefinisikan PERSEGI PANJANG render.", SW-740, SH-274, 15, WHITE);
        DrawText("Piksel DI LUAR persegi → tidak digambar (dibuang).", SW-740, SH-252, 15, WHITE);
        DrawText("Piksel DI DALAM persegi → digambar normal.", SW-740, SH-230, 15, WHITE);
        DrawText("Parameter: (x, y, lebar, tinggi) dalam piksel layar.", SW-740, SH-208, 15,
                 (Color){180,220,255,255});
        DrawText("BeginScissorMode(x, y, w, h)", SW-740, SH-183, 15, (Color){180,220,255,255});
        DrawText("  ...drawKereta(...)  <- hanya area scissor yang render", SW-740, SH-161, 14,
                 (Color){200,200,200,200});
        DrawText("EndScissorMode()", SW-740, SH-139, 15, (Color){180,220,255,255});
 
        // Swatch fase aktif
        const char* faseLabel[] = {"BEBAS (tanpa clip)", "WARP / ARRIVAL", "WARP_LOOP (diapit)"};
        Color faseCol[] = {
            (Color){80,220,80,255},
            (Color){255,180,50,255},
            (Color){255,80,80,255}
        };
        DrawRectangle(SW-740, SH-110, 18, 18, faseCol[clipMode]);
        DrawText(TextFormat("Mode aktif: %s", faseLabel[clipMode]),
                 SW-714, SH-109, 15, faseCol[clipMode]);
        DrawText(TextFormat("trainX = %.0f   clipMode = %d", trainX, clipMode),
                 SW-740, SH-82, 14, (Color){255,220,80,200});
        DrawText(TextFormat("tNorm  = %.2f  (loop %.1f detik)", tNorm, period),
                 SW-740, SH-60, 14, (Color){180,180,180,180});
    }
}