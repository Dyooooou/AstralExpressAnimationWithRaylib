#include "raylib.h"
#include "src/algo/bresenham.h"
#include "src/algo/dda.h"
#include "src/algo/midcircle.h"
#include "src/core/state.h"
#include "src/core/utils.h"
#include "src/world/bintang.h"
#include "src/world/planet.h"
#include "src/world/portal.h"
#include "src/world/kereta.h"
#include "src/screens/menu.h"
#include <stdlib.h>
#include <math.h>

int main(void) {
    InitWindow(SW, SH, "Simulasi Warp Jump - Astral Express");
    SetTargetFPS(60);

    initBintang();
    initTrail();
    randomizePlanets();

    Fase  fase         = MAIN_MENU;
    float faseTimer    = 0.0f;
    float warpFactor   = 0.0f;
    float portalOpen   = 0.0f;
    float rodaTheta    = 0.0f;
    float leftPortalX  = 150.0f;
    float leftPortalOpen = 0.0f;
    float keretaX      = 260.0f, keretaY = SH / 2.0f;
    float keretaAngle  = 0.0f;
    float portalX      = SW * 0.72f, portalY = SH / 2.0f;
    float portalRot    = 0.0f;
    float startX       = 0.0f, startY = 0.0f;
    bool  showHUD      = true;

    int  targetWarpCount  = 1;
    int  currentWarpCount = 0;
    char inputText[10]    = "\0";
    int  letterCount      = 0;

    while (!WindowShouldClose()) {
        float dt   = GetFrameTime();
        float time = GetTime();

        // ── Input: Main Menu ────────────────────────────────────
        if (fase == MAIN_MENU) {
            if (IsKeyPressed(KEY_DOWN))  menuOption = (menuOption + 1) % 4;
            if (IsKeyPressed(KEY_UP))    menuOption = (menuOption + 3) % 4;
            if (IsKeyPressed(KEY_ENTER)) {
                if      (menuOption == 0) fase = INPUT_WARP;
                else if (menuOption == 1) fase = MENU_GUIDE;
                else if (menuOption == 2) fase = MENU_MEKANISME;
                else                      break;
            }
        }

        // ── Input: Sub-menu ─────────────────────────────────────
        else if (fase == MENU_GUIDE || fase == MENU_MEKANISME) {
            if (fase == MENU_MEKANISME) {
                mekanismeTimer += dt;
                if (IsKeyPressed(KEY_RIGHT)) mekanismeSlide = (mekanismeSlide + 1) % 4;
                if (IsKeyPressed(KEY_LEFT))  mekanismeSlide = (mekanismeSlide + 2) % 4;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE))
                fase = MAIN_MENU;
        }

        // ── Input: Simulasi ─────────────────────────────────────
        if (IsKeyPressed(KEY_SPACE) && fase == IDLE) {
            fase = CHARGE; faseTimer = 0.0f;
            startX = keretaX; startY = keretaY;
        }
        if (IsKeyPressed(KEY_R)) {
            fase = IDLE; faseTimer = 0.0f;
            warpFactor = 0; portalOpen = 0;
            keretaX = 260; keretaY = SH / 2; keretaAngle = 0;
            rodaTheta = 0;
            initBintang();
        }
        if (IsKeyPressed(KEY_H)) showHUD = !showHUD;
        if (IsKeyPressed(KEY_T)) showOutline = !showOutline;

        // ── Input: Input Warp ────────────────────────────────────
        if (fase == INPUT_WARP) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48) && (key <= 57) && (letterCount < 5)) {
                    inputText[letterCount]     = (char)key;
                    inputText[letterCount + 1] = '\0';
                    letterCount++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                inputText[letterCount] = '\0';
            }
            if (IsKeyPressed(KEY_ENTER) && letterCount > 0) {
                targetWarpCount  = atoi(inputText);
                currentWarpCount = 0;
                fase             = IDLE;
                faseTimer        = 0.0f;
                startX           = keretaX;
                randomizePlanets();
            }
        }

        // ── Update Timer & Rotasi ────────────────────────────────
        faseTimer += dt;
        portalRot += dt * (1.0f + warpFactor * 3.0f);

        float omega = 2.0f + warpFactor * 12.0f;
        rodaTheta  -= omega * dt;

        // ── State Machine Simulasi ───────────────────────────────
        if (fase == IDLE) {
            keretaY     = SH / 2.0f + 6.0f * sinf(time * 1.0f);
            keretaAngle = 0.02f * sinf(time * 0.7f);
            warpFactor  = 0; portalOpen = 0;
        }
        else if (fase == CHARGE) {
            portalOpen  = clamp01(faseTimer / 2.0f);
            warpFactor  = clamp01(faseTimer / 3.0f) * 0.3f;
            keretaY     = SH / 2.0f + 4.0f * sinf(time * 2.0f);
            keretaX     = startX + clamp01(faseTimer / 2.0f) * 50.0f;
            if (faseTimer >= 2.5f) {
                fase = WARP; faseTimer = 0.0f;
                startX = keretaX; startY = keretaY;
            }
        }
        else if (fase == WARP) {
            float targetX = portalX + 800.0f;
            float dur     = 3.2f;
            float t       = clamp01(faseTimer / dur);
            float ease    = t * t;

            keretaX     = lerpF(startX, targetX, ease);
            keretaY     = lerpF(startY, portalY, ease) - 30.0f * sinf(t * PI);
            warpFactor  = lerpF(0.3f, 1.0f, clamp01(t * 1.5f));
            keretaAngle = lerpF(0.0f, -0.05f, t);

            if ((keretaX - 600.0f) > portalX) {
                currentWarpCount++;
                randomizePlanets();
                if (currentWarpCount < targetWarpCount) {
                    fase = WARP_LOOP; faseTimer = 0.0f;
                } else {
                    fase = ARRIVAL; faseTimer = 0.0f;
                }
            }
        }
        else if (fase == WARP_LOOP) {
            float dur     = 2.0f;
            float t       = clamp01(faseTimer / dur);
            float tPortal = clamp01(t / 0.3f);
            leftPortalOpen = lerpF(0.0f, 1.0f, tPortal);

            float tKereta = clamp01((t - 0.3f) / 0.7f);
            keretaX     = lerpF(leftPortalX - 800.0f, portalX + 800.0f, tKereta);
            keretaY     = portalY;
            warpFactor  = 1.0f;
            keretaAngle = -0.05f;

            if ((keretaX - 600.0f) > portalX) {
                currentWarpCount++;
                randomizePlanets();
                if (currentWarpCount < targetWarpCount) {
                    fase = WARP_LOOP; faseTimer = 0.0f;
                } else {
                    fase = ARRIVAL; faseTimer = 0.0f;
                }
            }
        }
        else if (fase == ARRIVAL) {
            float dur     = 3.5f;
            float t       = clamp01(faseTimer / dur);
            float tPortal = clamp01(t / 0.2f);
            leftPortalOpen = lerpF(0.0f, 1.0f, tPortal);

            float tKereta = clamp01((t - 0.2f) / 0.8f);
            float easeOut = 1.0f - (1.0f - tKereta) * (1.0f - tKereta);

            keretaX     = lerpF(leftPortalX - 800.0f, SW / 2.0f, easeOut);
            keretaY     = lerpF(portalY, startY, easeOut);
            warpFactor  = lerpF(1.0f, 0.0f, easeOut);
            keretaAngle = lerpF(-0.05f, 0.0f, easeOut);
            portalOpen     = lerpF(1.0f, 0.0f, tKereta);
            leftPortalOpen = lerpF(1.0f, 0.0f, tKereta);

            if (faseTimer >= dur) fase = DONE;
        }
        else if (fase == DONE) {
            if (IsKeyPressed(KEY_ENTER)) {
                fase        = INPUT_WARP;
                letterCount = 0;
                inputText[0] = '\0';
            }
        }

        // ── Kamera & Getaran ─────────────────────────────────────
        float portalDistance   = fminf(fabsf(keretaX - portalX), fabsf(keretaX - leftPortalX));
        float portalProximity  = clamp01(1.0f - portalDistance / 450.0f);
        float shakeWeight      = warpFactor * (0.2f + 0.8f * portalProximity);
        float shakeMagnitude   = 1.0f + shakeWeight * 14.0f;
        if (shakeMagnitude < 0.0f) shakeMagnitude = 0.0f;
        if (shakeMagnitude > 18.0f) shakeMagnitude = 18.0f;
        Vector2 shakeOffset    = {
            (GetRandomValue(-100, 100) / 100.0f) * shakeMagnitude,
            (GetRandomValue(-100, 100) / 100.0f) * shakeMagnitude
        };

        // ── Update Dunia ─────────────────────────────────────────
        updateBintang(warpFactor, dt);
        updatePlanets(warpFactor, dt);

        updateTrail(warpFactor, dt);
        if (warpFactor > 0.1f) {
            // Ekor kereta berada di belakang keretaX. 
            // Angka -150 (X) dan +30 (Y) ini adalah perkiraan posisi knalpot/belakang kereta.
            // Silakan ubah angkanya jika posisinya kurang pas dengan gambar keretamu.
            spawnTrailParticle(keretaX - 150.0f, keretaY + 30.0f, warpFactor); 
        }
        drawTrail(shakeOffset);

        // ── Render ───────────────────────────────────────────────
        BeginDrawing();
        ClearBackground((Color){3, 5, 18, 255});

        if (fase != MENU_MEKANISME) {
            drawBintang(warpFactor, shakeOffset);
            for (int i = 0; i < NUM_PLANETS; i++)
                drawPlanet(
                    planets[i].x + shakeOffset.x,
                    planets[i].y + shakeOffset.y,
                    planets[i].radius,
                    planets[i].color
                );
        }

        if      (fase == MAIN_MENU)       drawMainMenu();
        else if (fase == MENU_GUIDE)      drawGuide();
        else if (fase == MENU_MEKANISME)  drawMekanisme(mekanismeTimer);
        else {
            // ── Portal Kanan ─────────────────────────────────────
            drawPortal(portalX + shakeOffset.x, portalY + shakeOffset.y, 110.0f, portalRot, portalOpen);

            // ── Portal Kiri ──────────────────────────────────────
            if (leftPortalOpen > 0.0f)
                drawPortal(leftPortalX + shakeOffset.x, portalY + shakeOffset.y, 110.0f, -portalRot, leftPortalOpen);

            // ── Kereta (pass pertama dengan clipping) ────────────
            if (fase == IDLE || fase == CHARGE || fase == DONE) {
                drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
            } else if (fase == WARP) {
                float kepalX = keretaX + 300.0f;
                if (kepalX > portalX) {
                    BeginScissorMode(0, 0, (int)portalX, SH);
                    drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                    EndScissorMode();
                } else {
                    drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                }
            } else if (fase == WARP_LOOP) {
                BeginScissorMode((int)leftPortalX, 0, (int)(portalX - leftPortalX), SH);
                drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                EndScissorMode();
            } else if (fase == ARRIVAL) {
                BeginScissorMode((int)leftPortalX, 0, SW - (int)leftPortalX, SH);
                drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                EndScissorMode();
            }

            // ── Portal Kiri (overlay agar menutup ekor kereta) ───
            if (leftPortalOpen > 0.0f)
                drawPortal(leftPortalX + shakeOffset.x, portalY + shakeOffset.y, 110.0f, -portalRot, leftPortalOpen);

            // ── Kereta (pass kedua, identik, untuk render ulang
            //    di atas portal kiri yang baru digambar) ──────────
            if (fase == IDLE || fase == CHARGE || fase == DONE) {
                drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
            } else if (fase == WARP) {
                float kepalX = keretaX + 300.0f;
                if (kepalX > portalX) {
                    BeginScissorMode(0, 0, (int)portalX, SH);
                    drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                    EndScissorMode();
                } else {
                    drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                }
            } else if (fase == WARP_LOOP) {
                BeginScissorMode((int)leftPortalX, 0, (int)(portalX - leftPortalX), SH);
                drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                EndScissorMode();
            } else if (fase == ARRIVAL) {
                BeginScissorMode((int)leftPortalX, 0, SW - (int)leftPortalX, SH);
                drawKereta(keretaX + shakeOffset.x, keretaY + shakeOffset.y, keretaAngle, warpFactor, rodaTheta);
                EndScissorMode();
            }

            // ── HUD ──────────────────────────────────────────────
            if (showHUD) {
                const char *namaFase[] = {
                    "MAIN_MENU", "MENU_GUIDE", "MENU_MEKANISME",
                    "INPUT_WARP", "IDLE", "CHARGE", "WARP",
                    "WARP_LOOP", "ARRIVAL", "DONE"
                };
                DrawRectangle(10, 10, 310, 155, (Color){0, 0, 0, 140});
                DrawRectangleLines(10, 10, 310, 155, (Color){0, 180, 255, 80});
                DrawText("Astral Express - Warp Jump",                             20,  18, 16, (Color){0, 210, 255, 255});
                DrawText(TextFormat("Fase       : %s",       namaFase[fase]),      20,  42, 13, WHITE);
                DrawText(TextFormat("Warp Factor: %.2f",     warpFactor),          20,  60, 13, WHITE);
                DrawText(TextFormat("Portal     : %.2f",     portalOpen),          20,  78, 13, WHITE);
                DrawText(TextFormat("Roda theta : %.1f deg", rodaTheta*180/PI),    20,  96, 13, WHITE);
                DrawText(TextFormat("FPS        : %d", GetFPS()),                  20, 154, 13, GREEN);
                DrawText("[SPACE] Warp   [R] Reset   [H] HUD", 18, SH - 28, 13, (Color){160, 200, 255, 200});
            }
            if (showHUD) {
                int barWidth = 260;
                int barHeight = 14;
                int barX = (SW - barWidth) / 2;
                int barY = SH - 50;
                int labelY = barY - 18;
                int percentX = barX + barWidth + 10;
                DrawRectangle(barX, barY, barWidth, barHeight, (Color){20, 40, 65, 220});
                DrawRectangle(barX, barY, (int)(clamp01(warpFactor) * barWidth), barHeight, (Color){0, 180, 255, 220});
                DrawRectangleLines(barX, barY, barWidth, barHeight, (Color){0, 200, 255, 180});
                DrawText("Warp Meter", barX, labelY, 13, WHITE);
                DrawText(TextFormat("%d%%", (int)(clamp01(warpFactor) * 100.0f)), percentX, barY, 13, WHITE);
            }
        }

        // ── UI: Input Warp ───────────────────────────────────────
        if (fase == INPUT_WARP) {
            DrawRectangle(0, 0, SW, SH, (Color){10, 15, 30, 200});

            int boxWidth = 500, boxHeight = 250;
            int boxX = (SW - boxWidth) / 2;
            int boxY = (SH - boxHeight) / 2;

            DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){20, 30, 50, 255});
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, (Color){0, 200, 255, 255});
            DrawText("ASTRAL EXPRESS NAVIGATOR",     boxX + 50, boxY + 30,  28, (Color){0, 200, 255, 255});
            DrawText("Masukkan Jumlah Warp Jump:",   boxX + 50, boxY + 90,  20, WHITE);

            DrawRectangle(boxX + 50, boxY + 130, 400, 50, BLACK);
            DrawRectangleLines(boxX + 50, boxY + 130, 400, 50, GRAY);
            DrawText(inputText, boxX + 65, boxY + 145, 24, YELLOW);

            if (((int)(GetTime() * 2)) % 2 == 0) {
                int textWidth = MeasureText(inputText, 24);
                DrawText("_", boxX + 65 + textWidth, boxY + 145, 24, YELLOW);
            }
            DrawText("Tekan ENTER untuk memulai", boxX + 50, boxY + 200, 16, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}