// Simulasi Warp Jump - Astral Express
// Topik: Simulasi Realitas dalam Bidang 2D
// Objek: Kereta (Astral Express), Portal Warp, Bintang, Planet
// Primitif: Segitiga, Persegi, Lingkaran

#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define SW 1920
#define SH  1080
#define NUM_STARS 150
#ifndef PI
#define PI 3.14159f
#endif

typedef enum { IDLE, CHARGE, WARP, DONE } Fase;

// ── Bintang parallax ──────────────────────────────────────────
typedef struct { float x, y, size, speed; } Bintang;
Bintang bintang[NUM_STARS];

void initBintang() {
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
        if (warpFactor > 0.2f) {
            float streak = warpFactor * bintang[i].speed * 0.3f;
            DrawLineEx((Vector2){bintang[i].x, bintang[i].y},
                       (Vector2){bintang[i].x + streak, bintang[i].y},
                       bintang[i].size * 0.7f, (Color){200,220,255,200});
        } else {
            DrawCircleV((Vector2){bintang[i].x, bintang[i].y},
                        bintang[i].size, (Color){200,220,255,180});
        }
    }
}

// ── Planet ────────────────────────────────────────────────────
void drawPlanet(float cx, float cy, float r, Color warna) {
    DrawCircle(cx, cy, r, warna);
    DrawCircle(cx - r*0.3f, cy - r*0.3f, r*0.55f,
               (Color){warna.r+40, warna.g+40, warna.b+40, 80});
    DrawCircle(cx + r*0.2f, cy + r*0.2f, r*0.7f, (Color){0,0,0,60});
    DrawCircleLines(cx, cy, r, (Color){255,255,255,40});
}

// ── Portal Warp ───────────────────────────────────────────────
void drawPortal(float cx, float cy, float r, float rotasi, float openFactor) {
    if (openFactor <= 0.01f) return;
    float rad = r * openFactor;
    DrawCircle(cx, cy, rad+15, (Color){0,150,255,30});
    DrawCircle(cx, cy, rad+8,  (Color){0,180,255,50});
    DrawCircle(cx, cy, rad-3,  (Color){0,8,25,240});
    DrawCircleLines(cx, cy, rad,   (Color){0,220,255,200});
    DrawCircleLines(cx, cy, rad-4, (Color){100,230,255,150});
    for (int i = 0; i < 8; i++) {
        float sudut = rotasi + (i * 2*PI/8);
        float x1 = cx + rad * cosf(sudut);
        float y1 = cy + rad * sinf(sudut);
        float x2 = cx + (rad*0.5f) * cosf(sudut+0.5f);
        float y2 = cy + (rad*0.5f) * sinf(sudut+0.5f);
        DrawLineEx((Vector2){x1,y1},(Vector2){x2,y2},
                   1.5f,(Color){150,230,255,(unsigned char)(180*openFactor)});
        DrawCircle(x1, y1, 3, (Color){200,240,255,(unsigned char)(200*openFactor)});
    }
    DrawCircle(cx, cy, rad*0.2f,  (Color){0,150,255,100});
    DrawCircle(cx, cy, rad*0.08f, (Color){200,240,255,200});
}

// ════════════════════════════════════════════════════════════════
//  RODA KERETA dengan CONNECTING ROD
//
//  Anatomi roda lokomotif:
//    - Rim (cincin luar)        → DrawCircleLines
//    - Jari-jari (spoke)        → garis dari pusat ke rim, dirotasi
//    - Hub (pusat roda)         → lingkaran kecil
//    - Crankpin                 → titik kecil di tepi, ikut rotasi roda
//    - Connecting rod           → batang yang menghubungkan crankpin
//                                 roda depan & roda belakang
//    - Piston rod (opsional)    → batang horizontal dari crankpin ke depan
//
//  MATEMATIKA CONNECTING ROD:
//    Crankpin roda i:
//      pinX = cx + R * cos(θ + offset_i)
//      pinY = cy + R * sin(θ + offset_i)
//    Lalu DrawLineEx(pin_depan, pin_belakang) = connecting rod
// ════════════════════════════════════════════════════════════════

// Gambar satu roda beserta spoke-nya
// cx,cy = pusat roda, r = radius, theta = sudut rotasi
void drawRoda(float cx, float cy, float r, float theta, Color rimColor) {
    // Ban / rim luar (lingkaran tebal)
    DrawCircle(cx, cy, r,     (Color){30,30,35,255});     // ban hitam
    DrawCircleLines(cx, cy, r,     rimColor);
    DrawCircleLines(cx, cy, r-1.5f, rimColor);

    // Jari-jari (spoke) — 6 buah, dirotasi sesuai theta
    // Rumus: ujung spoke = (cx + r*cos(theta + i*60°), cy + r*sin(theta + i*60°))
    int numSpoke = 6;
    for (int i = 0; i < numSpoke; i++) {
        float sudut = theta + i * (2*PI / numSpoke);
        float sx = cx + (r - 2) * cosf(sudut);
        float sy = cy + (r - 2) * sinf(sudut);
        DrawLineEx((Vector2){cx, cy}, (Vector2){sx, sy},
                   1.8f, (Color){rimColor.r, rimColor.g, rimColor.b, 180});
    }

    // Hub tengah (lingkaran kecil)
    DrawCircle(cx, cy, r*0.18f, (Color){60,60,70,255});
    DrawCircleLines(cx, cy, r*0.18f, rimColor);

    // Flange (tonjolan dalam di kereta sungguhan) — cincin dalam
    DrawCircleLines(cx, cy, r*0.88f, (Color){rimColor.r,rimColor.g,rimColor.b,80});
}

// Gambar crankpin (titik koneksi connecting rod di tepi roda)
// Posisi: (cx + crankR * cos(theta), cy + crankR * sin(theta))
Vector2 getCrankPin(float cx, float cy, float crankR, float theta) {
    return (Vector2){
        cx + crankR * cosf(theta),
        cy + crankR * sinf(theta)
    };
}

// ════════════════════════════════════════════════════════════════
//  KERETA ASTRAL EXPRESS
//
//  Susunan (kiri ke kanan):
//    [Tender/Ekor] - [Badan Kereta] - [Ruang Mesin/Boiler] - [Cerobong/Kepala]
//
//  Roda:
//    - 2 roda besar di tengah (driving wheels) — ada connecting rod
//    - 1 roda kecil di belakang (trailing wheel)
//    - 1 roda kecil di depan   (leading wheel)
//
//  Warp stretch: kereta memanjang horizontal saat masuk portal
//    sx = 1 + warpFactor * 3
//    sy = 1 - warpFactor * 0.3
// ════════════════════════════════════════════════════════════════
void drawKereta(float cx, float cy, float angle, float warpFactor, float rodaTheta) {
    float sx = 1.0f + warpFactor * 3.0f;
    float sy = 1.0f - warpFactor * 0.3f;
    float scale = 1.5f; // ← ubah ini (1.0 = normal)

    float cosA = cosf(angle), sinA = sinf(angle);

    // Transformasi titik lokal → dunia
    // #define T(lx, ly) (Vector2){ \
    //     cx + ((lx)*sx)*cosA - ((ly)*sy)*sinA, \
    //     cy + ((lx)*sx)*sinA + ((ly)*sy)*cosA  \
    // }

    #define T(lx, ly) (Vector2){ \
    cx + ((lx * scale)*sx)*cosA - ((ly * scale)*sy)*sinA, \
    cy + ((lx * scale)*sx)*sinA + ((ly * scale)*sy)*cosA  \
    }
    // Transformasi hanya untuk posisi (tanpa skala, untuk roda)
    // #define TPOS(lx, ly) (Vector2){ \
    //     cx + (lx)*cosA - (ly)*sinA, \
    //     cy + (lx)*sinA + (ly)*cosA  \
    // }

    #define TPOS(lx, ly) (Vector2){ \
    cx + (lx * scale)*cosA - (ly * scale)*sinA, \
    cy + (lx * scale)*sinA + (ly * scale)*cosA  \
    }
    // ── Warna tema Astral Express ─────────────────────────────
    Color merahGelap  = (Color){160, 30,  30,  255};
    Color merahTerang = (Color){200, 50,  50,  255};
    Color krem        = (Color){220, 200, 150, 255};
    Color kremGelap   = (Color){180, 160, 110, 255};
    Color hitamBaja   = (Color){35,  35,  40,  255};
    Color chrome      = (Color){180, 185, 195, 255};
    Color emas        = (Color){200, 160, 60,  255};
    Color emasTerang  = (Color){240, 200, 80,  255};

    // ════════════════════════════════════════════════════════
    // [1] BADAN UTAMA KERETA (persegi panjang merah)
    //     Digambar sebagai 2 segitiga
    // ════════════════════════════════════════════════════════
    // Badan belakang (gerbong)
    Vector2 b1 = T(-95, -22);
    Vector2 b2 = T(-95,  18);
    Vector2 b3 = T( 20,  18);
    Vector2 b4 = T( 20, -22);
    DrawTriangle(b1, b2, b3, merahGelap);
    DrawTriangle(b1, b3, b4, merahGelap);

    // Stripe / panel dekorasi emas di badan
    Vector2 s1 = T(-95, -22);
    Vector2 s2 = T(-95, -17);
    Vector2 s3 = T( 20, -17);
    Vector2 s4 = T( 20, -22);
    DrawTriangle(s1, s2, s3, emas);
    DrawTriangle(s1, s3, s4, emas);

    Vector2 s5 = T(-95, 13);
    Vector2 s6 = T(-95, 18);
    Vector2 s7 = T( 20, 18);
    Vector2 s8 = T( 20, 13);
    DrawTriangle(s5, s6, s7, emas);
    DrawTriangle(s5, s7, s8, emas);

    // ════════════════════════════════════════════════════════
    // [2] KEPALA LOKOMOTIF / BOILER (depan, lebih besar)
    //     Bentuk: trapesium miring ke depan (segitiga x2)
    // ════════════════════════════════════════════════════════
    Vector2 k1 = T( 20, -26);
    Vector2 k2 = T( 20,  18);
    Vector2 k3 = T( 70,  18);
    Vector2 k4 = T( 75, -22);
    DrawTriangle(k1, k2, k3, merahTerang);
    DrawTriangle(k1, k3, k4, merahTerang);

    // Panel mesin (persegi kecil di boiler)
    Vector2 p1 = T(22, -20);
    Vector2 p2 = T(22,  12);
    Vector2 p3 = T(65,  12);
    Vector2 p4 = T(68, -20);
    DrawTriangle(p1, p2, p3, hitamBaja);
    DrawTriangle(p1, p3, p4, hitamBaja);

    // ════════════════════════════════════════════════════════
    // [3] CEROBONG ASAP (depan atas)
    //     Persegi panjang + segitiga di atas
    // ════════════════════════════════════════════════════════
    // Batang cerobong
    Vector2 c1 = T(52, -22);
    Vector2 c2 = T(52, -38);
    Vector2 c3 = T(62, -38);
    Vector2 c4 = T(62, -22);
    DrawTriangle(c1, c2, c3, hitamBaja);
    DrawTriangle(c1, c3, c4, hitamBaja);
    // Kepala cerobong (melebar ke atas — trapesium)
    Vector2 ct1 = T(49, -38);
    Vector2 ct2 = T(49, -43);
    Vector2 ct3 = T(65, -43);
    Vector2 ct4 = T(65, -38);
    DrawTriangle(ct1, ct2, ct3, hitamBaja);
    DrawTriangle(ct1, ct3, ct4, hitamBaja);

    // ════════════════════════════════════════════════════════
    // [4] KUBAH UAP (steam dome) — lingkaran di atas boiler
    // ════════════════════════════════════════════════════════
    Vector2 dome = T(35, -22);
    DrawCircle(dome.x, dome.y, 10*sy, merahTerang);
    DrawCircleLines(dome.x, dome.y, 10*sy, emas);

    // ════════════════════════════════════════════════════════
    // [5] JENDELA KABIN
    // ════════════════════════════════════════════════════════
    // Jendela besar (kiri badan)
    Vector2 j1 = T(-75, -20);
    Vector2 j2 = T(-75,  -4);
    Vector2 j3 = T(-55,  -4);
    Vector2 j4 = T(-55, -20);
    DrawTriangle(j1, j2, j3, (Color){10, 180, 220, 200});
    DrawTriangle(j1, j3, j4, (Color){10, 180, 220, 200});
    DrawLineEx(j1, j4, 1.5f, emasTerang);
    DrawLineEx(j4, j3, 1.5f, emasTerang);
    DrawLineEx(j3, j2, 1.5f, emasTerang);
    DrawLineEx(j2, j1, 1.5f, emasTerang);

    // Jendela tengah
    Vector2 j5 = T(-45, -20);
    Vector2 j6 = T(-45,  -4);
    Vector2 j7 = T(-28,  -4);
    Vector2 j8 = T(-28, -20);
    DrawTriangle(j5, j6, j7, (Color){10, 180, 220, 180});
    DrawTriangle(j5, j7, j8, (Color){10, 180, 220, 180});
    DrawLineEx(j5, j8, 1.5f, emasTerang);
    DrawLineEx(j8, j7, 1.5f, emasTerang);
    DrawLineEx(j7, j6, 1.5f, emasTerang);
    DrawLineEx(j6, j5, 1.5f, emasTerang);

    // ════════════════════════════════════════════════════════
    // [6] CHASSIS / UNDERFRAME (bawah badan)
    //     Persegi panjang tipis berwarna baja
    // ════════════════════════════════════════════════════════
    Vector2 u1 = T(-95, 18);
    Vector2 u2 = T(-95, 24);
    Vector2 u3 = T( 75, 24);
    Vector2 u4 = T( 75, 18);
    DrawTriangle(u1, u2, u3, hitamBaja);
    DrawTriangle(u1, u3, u4, hitamBaja);
    DrawLineEx(u1, u4, 1.5f, chrome);

    // ════════════════════════════════════════════════════════
    // [7] COWCATCHER (pelindung depan — segitiga lancip)
    // ════════════════════════════════════════════════════════
    DrawTriangle(T(75, 18), T(90, 24), T(75, 24), (Color){120,120,130,255});
    DrawTriangle(T(75, 18), T(75, 24), T(90, 24), (Color){140,140,150,255});
    // Garis-garis cowcatcher
    for (int i = 0; i < 3; i++) {
        float lx = 76 + i*5;
        DrawLineEx(T(lx, 18), T(lx+7, 24), 1.2f, chrome);
    }

    // ════════════════════════════════════════════════════════
    // [8] LAMPU DEPAN (lingkaran)
    // ════════════════════════════════════════════════════════
    Vector2 lamp = T(78, -5);
    DrawCircle(lamp.x, lamp.y, 6*sy,  hitamBaja);
    DrawCircle(lamp.x, lamp.y, 5*sy,  (Color){255,240,180,230});
    DrawCircle(lamp.x, lamp.y, 3*sy,  (Color){255,255,220,255});

    // ════════════════════════════════════════════════════════
    // [9] RODA + CONNECTING ROD
    //
    //  Posisi roda (koordinat lokal, sumbu Y ke atas = negatif di layar):
    //    roda[0] = leading wheel (kecil, paling depan)   lx= 55, ly=24
    //    roda[1] = driving wheel depan (besar)           lx= 25, ly=24
    //    roda[2] = driving wheel belakang (besar)        lx=-20, ly=24
    //    roda[3] = trailing wheel (kecil, belakang)      lx=-65, ly=24
    //    roda[4] = trailing wheel (kecil, paling belakang) lx=-85, ly=24
    //
    //  Roda besar punya connecting rod:
    //    crankPin = pusat roda + crankRadius * (cos(theta), sin(theta))
    //    connecting rod = garis dari crankPin[1] ke crankPin[2]
    //    side rod      = garis dari crankPin[1] ke crankPin[2] (sama)
    //    piston rod    = dari crankPin[1] ke arah depan (lx+pistonLen, ly_pin)
    // ════════════════════════════════════════════════════════

    Color rodaColor = chrome;
    float crankR_besar = 10.0f;   // radius lingkaran crankpin di roda besar

    // Posisi lokal pusat roda
    float rodaLX[5] = { 55,  25, -20, -65, -85 };
    float rodaLY[5] = { 24,  24,  24,  24,  24  };
    float rodaR[5]  = { 8,   14,  14,   8,   8  };
    // Roda besar (index 1,2) berputar dengan rodaTheta
    // Roda kecil berputar lebih cepat (radius lebih kecil → angular speed lebih cepat)
    float thetaRoda[5];
    thetaRoda[0] = rodaTheta * (14.0f/8.0f);   // leading: lebih cepat
    thetaRoda[1] = rodaTheta;
    thetaRoda[2] = rodaTheta;
    thetaRoda[3] = rodaTheta * (14.0f/8.0f);
    thetaRoda[4] = rodaTheta * (14.0f/8.0f);

    // ── Gambar connecting rod & side rod DULU (di balik roda) ──

    // Hitung posisi dunia crankpin roda besar (index 1 dan 2)
    // Pin roda 1 (driving depan)
    Vector2 pusat1 = TPOS(rodaLX[1], rodaLY[1]);
    Vector2 pusat2 = TPOS(rodaLX[2], rodaLY[2]);
    Vector2 pin1   = (Vector2){
        pusat1.x + crankR_besar * cosf(thetaRoda[1]),
        pusat1.y + crankR_besar * sinf(thetaRoda[1])
    };
    Vector2 pin2   = (Vector2){
        pusat2.x + crankR_besar * cosf(thetaRoda[2]),
        pusat2.y + crankR_besar * sinf(thetaRoda[2])
    };

    // CONNECTING ROD: menghubungkan crankpin roda 1 dan roda 2
    // Ini adalah batang paling ikonik di lokomotif uap
    DrawLineEx(pin1, pin2, 4.5f, (Color){100,100,110,255}); // bayangan
    DrawLineEx(pin1, pin2, 3.0f, chrome);                   // batang utama
    // Baut di ujung connecting rod
    DrawCircle(pin1.x, pin1.y, 4, chrome);
    DrawCircle(pin1.x, pin1.y, 2, hitamBaja);
    DrawCircle(pin2.x, pin2.y, 4, chrome);
    DrawCircle(pin2.x, pin2.y, 2, hitamBaja);

    // PISTON ROD: dari crankpin roda depan menuju depan (slide horizontal)
    // Simulasi gerak maju-mundur piston mengikuti rotasi roda
    // pistonX = pusat1.x + crankR * cos(theta)  (sudah = pin1.x)
    // ujung piston tetap di depan (x tetap), hanya pin yang bergerak
    float pistonEndX = TPOS(90, rodaLY[1]).x;
    DrawLineEx(pin1, (Vector2){pistonEndX, pin1.y}, 2.5f, (Color){80,80,90,255});
    DrawLineEx(pin1, (Vector2){pistonEndX, pin1.y}, 1.5f, chrome);

    // Silinder piston (kotak di depan roda 1)
    Vector2 sv1 = T(70, 16);
    Vector2 sv2 = T(70, 23);
    Vector2 sv3 = T(80, 23);
    Vector2 sv4 = T(80, 16);
    DrawTriangle(sv1,sv2,sv3, (Color){50,52,60,255});
    DrawTriangle(sv1,sv3,sv4, (Color){50,52,60,255});
    DrawLineEx(sv1,sv4,1.0f,chrome);

    // ── Gambar semua roda ────────────────────────────────────
    for (int i = 0; i < 5; i++) {
        Vector2 wp = TPOS(rodaLX[i], rodaLY[i]);
        drawRoda(wp.x, wp.y, rodaR[i] * sy, thetaRoda[i], rodaColor);
    }

    // ── Ornamen / detail tambahan ────────────────────────────
    // Rel pemandu (handrail) di atas boiler
    DrawLineEx(T(20,-26), T(72,-26), 1.5f, emas);
    DrawLineEx(T(-95,-22), T(20,-22), 1.0f, emas);

    // Kopling depan (hook kecil)
    DrawLineEx(T(90, 20), T(98, 20), 2.5f, chrome);
    DrawLineEx(T(98, 17), T(98, 23), 2.5f, chrome);

    // Kopling belakang
    DrawLineEx(T(-95, 20), T(-103, 20), 2.5f, chrome);

    // Engine glow saat warp (dari kopling belakang)
    if (warpFactor > 0.1f) {
        Vector2 gpos = T(-103, 20);
        float gr = 5.0f + warpFactor * 10.0f;
        DrawCircle(gpos.x, gpos.y, gr+4, (Color){0,150,255,(unsigned char)(40*warpFactor)});
        DrawCircle(gpos.x, gpos.y, gr,   (Color){100,200,255,(unsigned char)(150*warpFactor)});
        DrawCircle(gpos.x, gpos.y, gr*0.4f, (Color){255,255,255,(unsigned char)(220*warpFactor)});
    }

    #undef T
    #undef TPOS
}

// ── Helper ────────────────────────────────────────────────────
float lerpF(float a, float b, float t) { return a + (b-a)*t; }
float clamp01(float t) { return t<0?0:(t>1?1:t); }

// ══════════════════════════════════════════════════════════════
int main(void) {
    InitWindow(SW, SH, "Simulasi Warp Jump - Astral Express");
    SetTargetFPS(60);
    initBintang();

    Fase  fase        = IDLE;
    float faseTimer   = 0.0f;
    float warpFactor  = 0.0f;
    float portalOpen  = 0.0f;
    float rodaTheta   = 0.0f;   // sudut rotasi roda (akumulasi)
    float keretaX     = 260.0f, keretaY = SH/2.0f;
    float keretaAngle = 0.0f;
    float portalX     = SW * 0.72f, portalY = SH/2.0f;
    float portalRot   = 0.0f;
    float startX, startY;
    bool showHUD = true;

    while (!WindowShouldClose()) {
        float dt   = GetFrameTime();
        float time = GetTime();

        // ── Input ───────────────────────────────────────────
        if (IsKeyPressed(KEY_SPACE) && fase == IDLE) {
            fase = CHARGE; faseTimer = 0.0f;
            startX = keretaX; startY = keretaY;
        }
        if (IsKeyPressed(KEY_R)) {
            fase = IDLE; faseTimer = 0.0f;
            warpFactor = 0; portalOpen = 0;
            keretaX = 260; keretaY = SH/2; keretaAngle = 0;
            rodaTheta = 0;
            initBintang();
        }
        if (IsKeyPressed(KEY_H)) showHUD = !showHUD;

        // ── Update ──────────────────────────────────────────
        faseTimer += dt;
        portalRot += dt * (1.0f + warpFactor * 3.0f);

        // Roda berputar makin cepat saat warp
        // Kecepatan sudut roda ~ kecepatan linear kereta / radius roda
        // omega = v / r,  v bertambah dengan warpFactor
        float omega = (2.0f + warpFactor * 12.0f);  // rad/s
        rodaTheta  -= omega * dt;   // minus = maju ke kanan (searah jarum jam)

        if (fase == IDLE) {
            keretaY     = SH/2.0f + 6.0f * sinf(time * 1.0f);
            keretaAngle = 0.02f * sinf(time * 0.7f);
            warpFactor = 0; portalOpen = 0;
        }
        else if (fase == CHARGE) {
            portalOpen  = clamp01(faseTimer / 2.0f);
            warpFactor  = clamp01(faseTimer / 3.0f) * 0.3f;
            keretaY     = SH/2.0f + 4.0f * sinf(time * 2.0f);
            keretaX     = startX + clamp01(faseTimer/2.0f) * 50.0f;
            if (faseTimer >= 2.5f) {
                fase = WARP; faseTimer = 0.0f;
                startX = keretaX; startY = keretaY;
            }
        }
        else if (fase == WARP) {
            // Kereta harus bergerak sampai EKOR (kiri) melewati portal.
            // Ekor = keretaX - 110.
            // Target: keretaX - 110 > portalX  →  keretaX > portalX + 110
            // Jadi targetX = portalX + 120 (sedikit lewat agar ekor hilang)
            float targetX = portalX + 120.0f;
            float dur     = 3.2f;   // lebih panjang agar masuk pelan-pelan
            float t       = clamp01(faseTimer / dur);
            float ease    = t * t;
            keretaX     = lerpF(startX, targetX, ease);
            keretaY     = lerpF(startY, portalY, ease) - 30.0f * sinf(t * PI);
            warpFactor  = lerpF(0.3f, 1.0f, clamp01(t * 1.5f));
            keretaAngle = lerpF(0.0f, -0.05f, t);
            if (faseTimer >= dur) { fase = DONE; faseTimer = 0.0f; }
        }
        else if (fase == DONE) {
            warpFactor = lerpF(1.0f, 0.0f, clamp01(faseTimer/1.5f));
            portalOpen = lerpF(1.0f, 0.0f, clamp01(faseTimer/1.5f));
            if (faseTimer > 2.5f) {
                fase = IDLE;
                keretaX = 260; keretaY = SH/2;
            }
        }

        updateBintang(warpFactor, dt);

        // ── Draw ────────────────────────────────────────────
        BeginDrawing();
        ClearBackground((Color){3,5,18,255});

        drawBintang(warpFactor);
        drawPlanet(980, 150, 70, (Color){60,40,110,255});
        drawPlanet( 80, 560, 45, (Color){35,75,55, 255});
        drawPortal(portalX, portalY, 110.0f, portalRot, portalOpen);

        // ── Gambar kereta dengan efek masuk portal ───────────────
        // Teknik: BeginScissorMode(x, y, w, h) = clipping rectangle
        //   → hanya piksel dalam kotak itu yang digambar
        //
        // Saat WARP, kita set clipX = portalX (garis portal).
        // Bagian kereta di KANAN portalX otomatis terpotong/hilang.
        // Hasilnya: kepala masuk duluan, ekor menyusul → efek "ditelan".
        //
        // Ditambah glow tipis di garis potongan agar terlihat natural.

        if (fase != DONE || faseTimer < 0.3f) {
            if (fase == WARP || fase == DONE) {
                float kepalX = keretaX + 100.0f;  // ujung depan kereta
                float clipX  = portalX;

                int cx_int = (int)clipX;
                if (cx_int > 0) {
                    BeginScissorMode(0, 0, cx_int, SH);
                        drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
                    EndScissorMode();
                }

                // Glow di tepi potongan: kereta "ditelan" portal
                if (kepalX >= clipX - 5.0f) {
                    float glowAlp = clamp01((kepalX - clipX + 5.0f) / 30.0f);
                    float glowH   = 50.0f;
                    DrawLineEx(
                        (Vector2){clipX, keretaY - glowH},
                        (Vector2){clipX, keretaY + glowH},
                        3.0f, (Color){150, 230, 255, (unsigned char)(200 * glowAlp)}
                    );
                    DrawCircle(clipX, keretaY, glowH * 0.6f,
                               (Color){0, 180, 255, (unsigned char)(40 * glowAlp)});
                    DrawCircle(clipX, keretaY, glowH * 0.3f,
                               (Color){100, 220, 255, (unsigned char)(80 * glowAlp)});
                }
            } else {
                drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            }
        }

        // ── HUD ─────────────────────────────────────────────
        if (showHUD) {
            DrawRectangle(10, 10, 310, 155, (Color){0,0,0,140});
            DrawRectangleLines(10, 10, 310, 155, (Color){0,180,255,80});
            const char* namaFase[] = {"IDLE","CHARGE","WARP","DONE"};
            DrawText("Astral Express - Warp Jump", 20, 18, 16, (Color){0,210,255,255});
            DrawText(TextFormat("Fase       : %s",       namaFase[fase]),         20,  42, 13, WHITE);
            DrawText(TextFormat("Warp Factor: %.2f",     warpFactor),             20,  60, 13, WHITE);
            DrawText(TextFormat("Portal     : %.2f",     portalOpen),             20,  78, 13, WHITE);
            DrawText(TextFormat("Roda theta : %.1f deg", rodaTheta*180/PI),       20,  96, 13, WHITE);
            DrawText(TextFormat("Kereta XY  : (%.0f,%.0f)", keretaX, keretaY),   20, 114, 13, WHITE);
            DrawText(TextFormat("FPS        : %d", GetFPS()),                     20, 132, 13, GREEN);
            DrawText("[SPACE] Warp   [R] Reset   [H] HUD",
                     18, SH-28, 13, (Color){160,200,255,200});
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}