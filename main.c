// Simulasi Warp Jump - Astral Express
// Topik: Simulasi Realitas dalam Bidang 2D
// Objek: Kereta (Astral Express), Portal Warp, Bintang, Planet
// Primitif: Segitiga, Persegi, Lingkaran

#include "raylib.h"
#include "src/algo/bresenham.h"
#include "src/algo/dda.h"
#include "src/algo/midcircle.h"
#include <math.h>
#include <stdlib.h>

#define SW 1920
#define SH  1080
#define NUM_STARS 150
#ifndef PI
#define PI 3.14159f
#endif

typedef enum { INPUT_WARP, IDLE, CHARGE, WARP, WARP_LOOP, ARRIVAL, DONE } Fase;

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
            int thick = (int)(bintang[i].size * 0.7f);
            if (thick < 1) thick = 1;
            Bres_ThickLine((int)bintang[i].x, (int)bintang[i].y,
                           (int)(bintang[i].x + streak), (int)bintang[i].y,
                           thick, (Color){200,220,255,200});
        } else {
            MidcircleFilled((int)bintang[i].x, (int)bintang[i].y,
                            (int)bintang[i].size, (Color){200,220,255,180});
        }
    }
}

// ── Planet ────────────────────────────────────────────────────
void drawPlanet(float cx, float cy, float r, Color warna) {
    MidcircleFilled((int)cx, (int)cy, (int)r, warna);
    MidcircleFilled((int)(cx - r*0.3f), (int)(cy - r*0.3f), (int)(r*0.55f),
                    (Color){warna.r+40, warna.g+40, warna.b+40, 80});
    MidcircleFilled((int)(cx + r*0.2f), (int)(cy + r*0.2f), (int)(r*0.7f), (Color){0,0,0,60});
    Midcircle((int)cx, (int)cy, (int)r, (Color){255,255,255,40});
}

// ════════════════════════════════════════════════════════════════
//  HELPER: Color Lerp (Gradasi Transisi Warna)
// ════════════════════════════════════════════════════════════════
Color MyColorLerp(Color c1, Color c2, float t) {
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;
    return (Color){
        (unsigned char)(c1.r + t * (c2.r - c1.r)),
        (unsigned char)(c1.g + t * (c2.g - c1.g)),
        (unsigned char)(c1.b + t * (c2.b - c1.b)),
        (unsigned char)(c1.a + t * (c2.a - c1.a))
    };
}


void getSkewedHexagonVertices(Vector2 center, float radius, float rotasi, Vector2* vertices) {
    // Relasi 6 titik segienam yang dibuat miring (skewed) di ruang unit, 
    // meniru bentuk pusaran lorong dimensi (vortex).
    Vector2 unitPoints[6] = {
        { 0.2f, -1.0f }, // Titik 1: Top
        { 0.9f, -0.4f }, // Titik 2: Top Right
        { 0.7f,  0.8f }, // Titik 3: Bottom Right
        {-0.1f,  0.9f }, // Titik 4: Bottom
        {-0.9f,  0.3f }, // Titik 5: Bottom Left
        {-0.6f, -0.7f }  // Titik 6: Top Left
    };
    
    float cosA = cosf(rotasi), sinA = sinf(rotasi);
    for (int i = 0; i < 6; i++) {
        // Terapkan radius / skala
        float vx = unitPoints[i].x * radius;
        float vy = unitPoints[i].y * radius;
        
        // Putar sesuai rotasi portal dan geser ke titik tengah (center)
        vertices[i].x = center.x + vx * cosA - vy * sinA;
        vertices[i].y = center.y + vx * sinA + vy * cosA;
    }
}

// ── Portal Warp (Segienam Miring dengan Gradasi) ─────────────────
void drawPortal(float cx, float cy, float r, float rotasi, float openFactor) {
    if (openFactor <= 0.01f) return;
    
    // Gradasi Warna: Biru (Mulai) -> Ungu/Pink Intens (Saat Terbuka Lebar)
    Color idleBase = (Color){0, 180, 255, 255};   // Biru energi
    Color warpBase = (Color){220, 40, 200, 255};  // Pink/Ungu cerah
    
    // Warna inti: Hitam pekat -> Ungu Gelap
    Color c_int = MyColorLerp((Color){5, 10, 25, 240}, (Color){40, 5, 50, 240}, openFactor);
    // Warna cahaya luar
    Color c_warp = MyColorLerp(idleBase, warpBase, openFactor);
    
    Vector2 pnts[6];

    // [1] Isi Segienam Berlapis untuk efek Vortex
    // Memecah segienam menjadi 4 segitiga pembentuk: (0,1,2), (0,2,3), (0,3,4), (0,4,5)
    
    // Lapis 1 (Terluar & Paling Transparan - Aura Portal)
    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) + 20, rotasi, pnts);
    for(int i = 1; i <= 4; i++) {
        FillTriangle(pnts[0], pnts[i], pnts[i+1], (Color){c_warp.r, c_warp.g, c_warp.b, 25});
    }

    // Lapis 2 (Bodi Cahaya Tengah)
    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) + 5, rotasi, pnts);
    for(int i = 1; i <= 4; i++) {
        FillTriangle(pnts[0], pnts[i], pnts[i+1], (Color){c_warp.r, c_warp.g, c_warp.b, 50});
    }

    // Lapis 3 (Inti Gelap Tempat Kereta Masuk)
    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) - 6, rotasi, pnts);
    for(int i = 1; i <= 4; i++) {
        FillTriangle(pnts[0], pnts[i], pnts[i+1], c_int);
    }

    // [2] Garis Pinggir Segienam (Hexagon Outlines)
    int thick = 3;
    
    // Garis Utama Luar
    getSkewedHexagonVertices((Vector2){cx, cy}, r * openFactor, rotasi, pnts);
    Color c_line = MyColorLerp((Color){0, 220, 255, 200}, (Color){255, 150, 220, 200}, openFactor);
    for (int i = 0; i < 6; i++) {
        Bres_ThickLine((int)pnts[i].x, (int)pnts[i].y,
        (int)pnts[(i+1)%6].x, (int)pnts[(i+1)%6].y,
                       thick, c_line);
    }
    
    // Garis Dalam (Sedikit diputar untuk memberi kesan 3D/Lorong)
    getSkewedHexagonVertices((Vector2){cx, cy}, (r * openFactor) - 8, rotasi + 0.15f, pnts);
    Color c_line_in = MyColorLerp((Color){150, 240, 255, 150}, (Color){255, 200, 240, 150}, openFactor);
    for (int i = 0; i < 6; i++) {
        Bres_ThickLine((int)pnts[i].x, (int)pnts[i].y, 
        (int)pnts[(i+1)%6].x, (int)pnts[(i+1)%6].y, 
                       2, c_line_in);
    }

    // [3] Titik Pusat (Singularity Kecil di Tengah)
    MidcircleFilled((int)cx, (int)cy, (int)((r * openFactor)*0.15f), 
                   (Color){c_warp.r, c_warp.g, c_warp.b, (unsigned char)(100*openFactor)});
    MidcircleFilled((int)cx, (int)cy, (int)((r * openFactor)*0.05f), 
                   (Color){255, 255, 255, (unsigned char)(200*openFactor)});
}


// ════════════════════════════════════════════════════════════════
//  RODA KERETA dengan CONNECTING ROD
// ════════════════════════════════════════════════════════════════
void drawRoda(float cx, float cy, float r, float theta, Color rimColor) {
    int icx = (int)cx, icy = (int)cy, ir = (int)r;

    MidcircleFilled(icx, icy, ir, (Color){30,30,35,255});
    MidcircleThick(icx, icy, ir, 1, rimColor);
    Midcircle(icx, icy, ir - 1, rimColor); // rim tebal

    int numSpoke = 6;
    for (int i = 0; i < numSpoke; i++) {
        float sudut = theta + i * (2*PI / numSpoke);
        int sx = (int)(cx + (r - 2) * cosf(sudut));
        int sy = (int)(cy + (r - 2) * sinf(sudut));
        Bres_ThickLine(icx, icy, sx, sy, 2, (Color){rimColor.r, rimColor.g, rimColor.b, 180});
    }

    int hubR = (int)(r * 0.18f);
    MidcircleFilled(icx, icy, hubR, (Color){60,60,70,255});
    Midcircle(icx, icy, hubR, rimColor);

    Midcircle(icx, icy, (int)(r * 0.88f), (Color){rimColor.r,rimColor.g,rimColor.b,80});
}

Vector2 getCrankPin(float cx, float cy, float crankR, float theta) {
    return (Vector2){
        cx + crankR * cosf(theta),
        cy + crankR * sinf(theta)
    };
}

// ════════════════════════════════════════════════════════════════
//  KERETA ASTRAL EXPRESS
// ════════════════════════════════════════════════════════════════
void drawKereta(float cx, float cy, float angle, float warpFactor, float rodaTheta) {
    float sx = 1.0f + warpFactor * 3.0f;
    float sy = 1.0f - warpFactor * 0.3f;
    float scale = 1.5f;

    float cosA = cosf(angle), sinA = sinf(angle);

    #define T(lx, ly) (Vector2){ \
    cx + ((lx * scale)*sx)*cosA - ((ly * scale)*sy)*sinA, \
    cy + ((lx * scale)*sx)*sinA + ((ly * scale)*sy)*cosA  \
    }

    #define TPOS(lx, ly) (Vector2){ \
    cx + (lx * scale)*cosA - (ly * scale)*sinA, \
    cy + (lx * scale)*sinA + (ly * scale)*cosA  \
    }

    Color merahGelap  = (Color){160, 30,  30,  255};
    Color merahTerang = (Color){200, 50,  50,  255};
    // Color krem        = (Color){220, 200, 150, 255};
    // Color kremGelap   = (Color){180, 160, 110, 255};
    Color hitamBaja   = (Color){35,  35,  40,  255};
    Color chrome      = (Color){180, 185, 195, 255};
    Color emas        = (Color){200, 160, 60,  255};
    Color emasTerang  = (Color){240, 200, 80,  255};

    // [1] BADAN UTAMA KERETA
    Vector2 b1 = T(-95, -22), b2 = T(-95,  18), b3 = T( 20,  18), b4 = T( 20, -22);
    FillTriangle(b1, b2, b3, merahGelap);
    FillTriangle(b1, b3, b4, merahGelap);

    Vector2 s1 = T(-95, -22), s2 = T(-95, -17), s3 = T( 20, -17), s4 = T( 20, -22);
    FillTriangle(s1, s2, s3, emas);
    FillTriangle(s1, s3, s4, emas);

    Vector2 s5 = T(-95, 13), s6 = T(-95, 18), s7 = T( 20, 18), s8 = T( 20, 13);
    FillTriangle(s5, s6, s7, emas);
    FillTriangle(s5, s7, s8, emas);

    // [2] KEPALA LOKOMOTIF / BOILER
    Vector2 k1 = T( 20, -26), k2 = T( 20,  18), k3 = T( 70,  18), k4 = T( 75, -22);
    FillTriangle(k1, k2, k3, merahTerang);
    FillTriangle(k1, k3, k4, merahTerang);

    Vector2 p1 = T(22, -20), p2 = T(22,  12), p3 = T(65,  12), p4 = T(68, -20);
    FillTriangle(p1, p2, p3, hitamBaja);
    FillTriangle(p1, p3, p4, hitamBaja);

    // [3] CEROBONG ASAP
    Vector2 c1 = T(52, -22), c2 = T(52, -38), c3 = T(62, -38), c4 = T(62, -22);
    FillTriangle(c1, c2, c3, hitamBaja);
    FillTriangle(c1, c3, c4, hitamBaja);
    
    Vector2 ct1 = T(49, -38), ct2 = T(49, -43), ct3 = T(65, -43), ct4 = T(65, -38);
    FillTriangle(ct1, ct2, ct3, hitamBaja);
    FillTriangle(ct1, ct3, ct4, hitamBaja);

    // [4] KUBAH UAP (steam dome)
    Vector2 dome = T(35, -22);
    MidcircleFilled((int)dome.x, (int)dome.y, (int)(10*sy), merahTerang);
    Midcircle((int)dome.x, (int)dome.y, (int)(10*sy), emas);

    // [5] JENDELA KABIN
    Vector2 j1 = T(-75, -20), j2 = T(-75,  -4), j3 = T(-55,  -4), j4 = T(-55, -20);
    FillTriangle(j1, j2, j3, (Color){10, 180, 220, 200});
    FillTriangle(j1, j3, j4, (Color){10, 180, 220, 200});
    Bres_ThickLine((int)j1.x, (int)j1.y, (int)j4.x, (int)j4.y, 2, emasTerang);
    Bres_ThickLine((int)j4.x, (int)j4.y, (int)j3.x, (int)j3.y, 2, emasTerang);
    Bres_ThickLine((int)j3.x, (int)j3.y, (int)j2.x, (int)j2.y, 2, emasTerang);
    Bres_ThickLine((int)j2.x, (int)j2.y, (int)j1.x, (int)j1.y, 2, emasTerang);

    Vector2 j5 = T(-45, -20), j6 = T(-45,  -4), j7 = T(-28,  -4), j8 = T(-28, -20);
    FillTriangle(j5, j6, j7, (Color){10, 180, 220, 180});
    FillTriangle(j5, j7, j8, (Color){10, 180, 220, 180});
    Bres_ThickLine((int)j5.x, (int)j5.y, (int)j8.x, (int)j8.y, 2, emasTerang);
    Bres_ThickLine((int)j8.x, (int)j8.y, (int)j7.x, (int)j7.y, 2, emasTerang);
    Bres_ThickLine((int)j7.x, (int)j7.y, (int)j6.x, (int)j6.y, 2, emasTerang);
    Bres_ThickLine((int)j6.x, (int)j6.y, (int)j5.x, (int)j5.y, 2, emasTerang);

    // [6] CHASSIS
    Vector2 u1 = T(-95, 18), u2 = T(-95, 24), u3 = T( 75, 24), u4 = T( 75, 18);
    FillTriangle(u1, u2, u3, hitamBaja);
    FillTriangle(u1, u3, u4, hitamBaja);
    Bres_ThickLine((int)u1.x, (int)u1.y, (int)u4.x, (int)u4.y, 2, chrome);

    // [7] COWCATCHER
    FillTriangle(T(75, 18), T(90, 24), T(75, 24), (Color){120,120,130,255});
    FillTriangle(T(75, 18), T(75, 24), T(90, 24), (Color){140,140,150,255});
    for (int i = 0; i < 3; i++) {
        float lx = 76 + i*5;
        Vector2 cw1 = T(lx, 18), cw2 = T(lx+7, 24);
        Bres_ThickLine((int)cw1.x, (int)cw1.y, (int)cw2.x, (int)cw2.y, 1, chrome);
    }

    // [8] LAMPU DEPAN
    Vector2 lamp = T(78, -5);
    MidcircleFilled((int)lamp.x, (int)lamp.y, (int)(6*sy),  hitamBaja);
    MidcircleFilled((int)lamp.x, (int)lamp.y, (int)(5*sy),  (Color){255,240,180,230});
    MidcircleFilled((int)lamp.x, (int)lamp.y, (int)(3*sy),  (Color){255,255,220,255});

    // [9] RODA + CONNECTING ROD
    Color rodaColor = chrome;
    float crankR_besar = 10.0f;

    float rodaLX[5] = { 55,  25, -20, -65, -85 };
    float rodaLY[5] = { 24,  24,  24,  24,  24  };
    float rodaR[5]  = { 8,   14,  14,   8,   8  };
    float thetaRoda[5];
    thetaRoda[0] = rodaTheta * (14.0f/8.0f);
    thetaRoda[1] = rodaTheta;
    thetaRoda[2] = rodaTheta;
    thetaRoda[3] = rodaTheta * (14.0f/8.0f);
    thetaRoda[4] = rodaTheta * (14.0f/8.0f);

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

    Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pin2.x, (int)pin2.y, 5, (Color){100,100,110,255});
    Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pin2.x, (int)pin2.y, 3, chrome);
    
    MidcircleFilled((int)pin1.x, (int)pin1.y, 4, chrome);
    MidcircleFilled((int)pin1.x, (int)pin1.y, 2, hitamBaja);
    MidcircleFilled((int)pin2.x, (int)pin2.y, 4, chrome);
    MidcircleFilled((int)pin2.x, (int)pin2.y, 2, hitamBaja);

    float pistonEndX = TPOS(90, rodaLY[1]).x;
    Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pistonEndX, (int)pin1.y, 3, (Color){80,80,90,255});
    Bres_ThickLine((int)pin1.x, (int)pin1.y, (int)pistonEndX, (int)pin1.y, 2, chrome);

    Vector2 sv1 = T(70, 16), sv2 = T(70, 23), sv3 = T(80, 23), sv4 = T(80, 16);
    FillTriangle(sv1,sv2,sv3, (Color){50,52,60,255});
    FillTriangle(sv1,sv3,sv4, (Color){50,52,60,255});
    Bres_ThickLine((int)sv1.x, (int)sv1.y, (int)sv4.x, (int)sv4.y, 1, chrome);

    for (int i = 0; i < 5; i++) {
        Vector2 wp = TPOS(rodaLX[i], rodaLY[i]);
        drawRoda(wp.x, wp.y, rodaR[i] * sy, thetaRoda[i], rodaColor);
    }

    Vector2 hr1 = T(20,-26), hr2 = T(72,-26);
    Vector2 hr3 = T(-95,-22), hr4 = T(20,-22);
    Bres_ThickLine((int)hr1.x, (int)hr1.y, (int)hr2.x, (int)hr2.y, 2, emas);
    Bres_ThickLine((int)hr3.x, (int)hr3.y, (int)hr4.x, (int)hr4.y, 1, emas);

    Vector2 cp1 = T(90, 20), cp2 = T(98, 20);
    Vector2 cp3 = T(98, 17), cp4 = T(98, 23);
    Bres_ThickLine((int)cp1.x, (int)cp1.y, (int)cp2.x, (int)cp2.y, 3, chrome);
    Bres_ThickLine((int)cp3.x, (int)cp3.y, (int)cp4.x, (int)cp4.y, 3, chrome);

    Vector2 cpb1 = T(-95, 20), cpb2 = T(-103, 20);
    Bres_ThickLine((int)cpb1.x, (int)cpb1.y, (int)cpb2.x, (int)cpb2.y, 3, chrome);

    if (warpFactor > 0.1f) {
        Vector2 gpos = T(-103, 20);
        float gr = 5.0f + warpFactor * 10.0f;
        MidcircleFilled((int)gpos.x, (int)gpos.y, (int)(gr+4), (Color){0,150,255,(unsigned char)(40*warpFactor)});
        MidcircleFilled((int)gpos.x, (int)gpos.y, (int)gr,   (Color){100,200,255,(unsigned char)(150*warpFactor)});
        MidcircleFilled((int)gpos.x, (int)gpos.y, (int)(gr*0.4f), (Color){255,255,255,(unsigned char)(220*warpFactor)});
    }

    #undef T
    #undef TPOS
}

float lerpF(float a, float b, float t) { return a + (b-a)*t; }
float clamp01(float t) { return t<0?0:(t>1?1:t); }

// ══════════════════════════════════════════════════════════════
int main(void) {
    InitWindow(SW, SH, "Simulasi Warp Jump - Astral Express");
    SetTargetFPS(60);
    initBintang();
    Fase fase = INPUT_WARP;
    // Fase  fase        = IDLE;
    float faseTimer   = 0.0f;
    float warpFactor  = 0.0f;
    float portalOpen  = 0.0f;
    float rodaTheta   = 0.0f;
    float leftPortalX = 150.0f;  // Posisi portal kiri
    float leftPortalOpen = 0.0f; // Skala ukuran portal kiri (0.0 sampai 1.0)
    float keretaX     = 260.0f, keretaY = SH/2.0f;
    float keretaAngle = 0.0f;
    float portalX     = SW * 0.72f, portalY = SH/2.0f;
    float portalRot   = 0.0f;
    float startX, startY;
    bool showHUD = true;

    // Variabel untuk Input & Loop Warp
    int targetWarpCount = 1;      // Berapa kali harus lompat
    int currentWarpCount = 0;     // Sudah lompat berapa kali
    char inputText[10] = "\0";    // Buffer teks input
    int letterCount = 0;          // Jumlah karakter input

    while (!WindowShouldClose()) {
        float dt   = GetFrameTime();
        float time = GetTime();

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

        faseTimer += dt;
        portalRot += dt * (1.0f + warpFactor * 3.0f);

        float omega = (2.0f + warpFactor * 12.0f);
        rodaTheta  -= omega * dt;

        if (fase == INPUT_WARP) {
            // Ambil input keyboard (Hanya angka)
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48) && (key <= 57) && (letterCount < 5)) { // ASCII 48-57 adalah angka 0-9
                    inputText[letterCount] = (char)key;
                    inputText[letterCount+1] = '\0';
                    letterCount++;
                }
                key = GetCharPressed();  // Cek jika ada tombol lain ditekan pada frame yang sama
            }

            // Hapus karakter (Backspace)
            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                inputText[letterCount] = '\0';
            }

            // Mulai Warp saat tekan ENTER
            if (IsKeyPressed(KEY_ENTER) && letterCount > 0) {
                targetWarpCount = atoi(inputText); // Ubah teks ke integer
                currentWarpCount = 0;
                fase = IDLE;        // Mulai animasi
                faseTimer = 0.0f;
            }
        }

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
        // ── 1. LOGIKA WARP (Lompatan Pertama: Tengah ke Kanan) ─
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
                if (currentWarpCount < targetWarpCount) {
                    fase = WARP_LOOP; // Lanjut loop (Kiri -> Kanan)
                    faseTimer = 0.0f;
                } else {
                    fase = ARRIVAL;   // Langsung mendarat jika inputnya 1
                    faseTimer = 0.0f;
                }
            }
        }
        // ── 2. LOGIKA WARP_LOOP (Lompatan Lanjutan: Kiri ke Kanan) ─
        else if (fase == WARP_LOOP) {
            float dur = 2.0f; 
            float t = clamp01(faseTimer / dur);
            
            // 0.0 - 0.3: Portal kiri membesar dari nol
            float tPortal = clamp01(t / 0.3f);
            leftPortalOpen = lerpF(0.0f, 1.0f, tPortal);
            
            // 0.3 - 1.0: Kereta menembak keluar
            float tKereta = clamp01((t - 0.3f) / 0.7f);
            keretaX = lerpF(leftPortalX - 800.0f, portalX + 800.0f, tKereta);
            keretaY = portalY; 
            
            warpFactor = 1.0f;   // Jaga kereta tetap stretch
            keretaAngle = -0.05f;
            
            // Cek jika ekor sudah masuk ke portal kanan lagi
            if ((keretaX - 600.0f) > portalX) {
                currentWarpCount++;
                if (currentWarpCount < targetWarpCount) {
                    fase = WARP_LOOP; // Ulangi terus
                    faseTimer = 0.0f;
                } else {
                    fase = ARRIVAL;   // Selesai, siapkan pendaratan
                    faseTimer = 0.0f;
                }
            }
        }
        // ── 3. LOGIKA ARRIVAL (Mendarat di TENGAH Layar) ────────
        else if (fase == ARRIVAL) {
            float dur = 3.5f;
            float t = clamp01(faseTimer / dur);
            
            // Pastikan portal kiri membesar (jika lompatan cuma 1, portal ini baru muncul)
            float tPortal = clamp01(t / 0.2f);
            leftPortalOpen = lerpF(0.0f, 1.0f, tPortal);
            
            // Gerakan mengerem menuju TENGAH layar (SW / 2.0f)
            float tKereta = clamp01((t - 0.2f) / 0.8f);
            float easeOut = 1.0f - (1.0f - tKereta) * (1.0f - tKereta);
            
            keretaX = lerpF(leftPortalX - 800.0f, SW / 2.0f, easeOut);
            keretaY = lerpF(portalY, startY, easeOut);
            
            // Kereta kembali ke bentuk normal
            warpFactor  = lerpF(1.0f, 0.0f, easeOut);
            keretaAngle = lerpF(-0.05f, 0.0f, easeOut);
            
            // Di akhir pendaratan, tutup kedua portal!
            portalOpen = lerpF(1.0f, 0.0f, tKereta);
            leftPortalOpen = lerpF(1.0f, 0.0f, tKereta);
            
            if (faseTimer >= dur) {
                fase = DONE;
            }
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

        BeginDrawing();
        ClearBackground((Color){3,5,18,255});

        drawBintang(warpFactor); 
        drawPlanet(980, 150, 70, (Color){60,40,110,255});
        drawPlanet( 80, 560, 45, (Color){35,75,55, 255});
        drawPortal(portalX, portalY, 110.0f, portalRot, portalOpen);

        // ── GAMBAR PORTAL KIRI ──────────────────────────────────
        if (leftPortalOpen > 0.0f) {
            float pw = 100.0f * leftPortalOpen;
            float ph = 400.0f * leftPortalOpen;
            // Warnanya kubedakan sedikit (biru cerah) agar terlihat epik
            DrawEllipse((int)leftPortalX, (int)portalY, pw, ph, (Color){50, 200, 255, 180});
            DrawEllipse((int)leftPortalX, (int)portalY, pw * 0.7f, ph * 0.7f, (Color){150, 230, 255, 255});
        }

        // ── GAMBAR KERETA DENGAN CLIPPING ───────────────────────
        if (fase == IDLE || fase == CHARGE || fase == DONE) {
            drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
        } 
        else if (fase == WARP) {
            float kepalX = keretaX + 300.0f;
            if (kepalX > portalX) {
                // Potong kepala kereta yang masuk ke portal kanan
                BeginScissorMode(0, 0, (int)portalX, SH);
                drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
                EndScissorMode();
            } else {
                drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            }
        } 
        else if (fase == WARP_LOOP) {
            // Kereta diapit! Potong ekor di portal kiri, dan potong kepala di portal kanan
            int clipStartX = (int)leftPortalX;
            int clipWidth = (int)(portalX - leftPortalX);
            BeginScissorMode(clipStartX, 0, clipWidth, SH);
            drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            EndScissorMode();
        } 
        else if (fase == ARRIVAL) {
            // Potong ekor yang baru keluar dari portal kiri saja
            int clipStartX = (int)leftPortalX;
            int clipWidth = SW - clipStartX;
            BeginScissorMode(clipStartX, 0, clipWidth, SH);
            drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            EndScissorMode();
        }

        if (leftPortalOpen > 0.0f) {
            // Kita pakai fungsi drawPortal segienam milikmu! 
            // Rotasinya diberi minus (-portalRot) agar putarannya berlawanan arah dengan portal kanan
            drawPortal(leftPortalX, portalY, 110.0f, -portalRot, leftPortalOpen);
        }

        // if (fase != DONE || faseTimer < 0.3f) {
        //     if (fase == WARP || fase == CHARGE || fase == ARRIVAL || fase == DONE) {
        //         float kepalX = keretaX + 300.0f;
        //         float clipX  = portalX;

        //         int cx_int = (int)clipX;
        //         if (cx_int > 0) {
        //             BeginScissorMode(0, 0, cx_int, SH);
        //                 drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
        //             EndScissorMode();
        //         }

        //         if (kepalX >= clipX - 5.0f) {
        //             float glowAlp = clamp01((kepalX - clipX + 5.0f) / 30.0f);
        //             float glowH   = 50.0f;
        //             Bres_ThickLine(
        //                 (int)clipX, (int)(keretaY - glowH),
        //                 (int)clipX, (int)(keretaY + glowH),
        //                 3, (Color){150, 230, 255, (unsigned char)(200 * glowAlp)}
        //             );
        //             MidcircleFilled((int)clipX, (int)keretaY, (int)(glowH * 0.6f),
        //                        (Color){0, 180, 255, (unsigned char)(40 * glowAlp)});
        //             MidcircleFilled((int)clipX, (int)keretaY, (int)(glowH * 0.3f),
        //                        (Color){100, 220, 255, (unsigned char)(80 * glowAlp)});
        //         }
        //     } else {
        //         drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
        //     }
        // }
        if (fase == IDLE || fase == CHARGE || fase == DONE) {
            drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
        } 
        else if (fase == WARP) {
            float kepalX = keretaX + 300.0f;
            if (kepalX > portalX) {
                // Potong kepala kereta saat masuk portal kanan
                BeginScissorMode(0, 0, (int)portalX, SH);
                drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
                EndScissorMode();
            } else {
                drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            }
        } 
        else if (fase == WARP_LOOP) {
            // Diapit 2 portal: Potong ekor di kiri, potong kepala di kanan
            int clipStartX = (int)leftPortalX;
            int clipWidth = (int)(portalX - leftPortalX);
            BeginScissorMode(clipStartX, 0, clipWidth, SH);
            drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            EndScissorMode();
        } 
        else if (fase == ARRIVAL) {
            // Potong ekor yang baru keluar dari portal kiri
            int clipStartX = (int)leftPortalX;
            int clipWidth = SW - clipStartX;
            BeginScissorMode(clipStartX, 0, clipWidth, SH);
            drawKereta(keretaX, keretaY, keretaAngle, warpFactor, rodaTheta);
            EndScissorMode();
        }

        // HUD (UI Info tetap menggunakan Raylib primitives)
        if (showHUD) {
            DrawRectangle(10, 10, 310, 155, (Color){0,0,0,140});
            DrawRectangleLines(10, 10, 310, 155, (Color){0,180,255,80});
            const char* namaFase[] = {"INPUT", "IDLE", "CHARGE", "WARP", "WARP_LOOP", "ARRIVAL", "DONE"};
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
        // ── RENDER MENU INPUT ──────────────────────────────────
        if (fase == INPUT_WARP) {
            // Buat background semi-transparan
            DrawRectangle(0, 0, SW, SH, (Color){10, 15, 30, 200});
            
            int boxWidth = 500;
            int boxHeight = 250;
            int boxX = (SW - boxWidth) / 2;
            int boxY = (SH - boxHeight) / 2;
            
            // Kotak Menu
            DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){20, 30, 50, 255});
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, (Color){0, 200, 255, 255});
            
            DrawText("ASTRAL EXPRESS NAVIGATOR", boxX + 50, boxY + 30, 28, (Color){0, 200, 255, 255});
            DrawText("Masukkan Jumlah Warp Jump:", boxX + 50, boxY + 90, 20, WHITE);
            
            // Kotak Teks
            DrawRectangle(boxX + 50, boxY + 130, 400, 50, BLACK);
            DrawRectangleLines(boxX + 50, boxY + 130, 400, 50, GRAY);
            DrawText(inputText, boxX + 65, boxY + 145, 24, YELLOW);
            
            // Cursor berkedip
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