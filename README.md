# 🚂 Astral Express — Warp Jump Simulator

> Simulasi lompatan warp kereta antarbintang dalam bidang 2D, dibangun di atas **raylib** dengan algoritma grafika komputer dari nol — tanpa fungsi primitif bawaan library.

---

## 📋 Daftar Isi

- [Gambaran Program](#gambaran-program)
- [Struktur Direktori](#struktur-direktori)
- [Cara Build & Menjalankan](#cara-build--menjalankan)
- [Kontrol](#kontrol)
- [Algoritma Grafika yang Diimplementasikan](#algoritma-grafika-yang-diimplementasikan)
- [Transformasi 2D yang Digunakan](#transformasi-2d-yang-digunakan)
- [Mekanisme Animasi](#mekanisme-animasi)
- [Alur State Machine](#alur-state-machine)

---

## Gambaran Program

Program ini mensimulasikan **Warp Jump** — lompatan supercepat menembus portal dimensi — dari perspektif samping (2D). Pengguna dapat menentukan berapa kali kereta melompat, lalu menyaksikan animasi lengkap: pengisian energi, masuk portal, melintasi ruang warp, hingga mendarat.

Seluruh objek — kereta, portal, bintang, planet — digambar menggunakan **algoritma rasterisasi manual** (Bresenham, DDA, Midpoint Circle) yang diimplementasikan sendiri, bukan fungsi `DrawLine` atau `DrawCircle` bawaan raylib.

Program juga dilengkapi **halaman Mekanisme** interaktif (4 slide) yang memvisualisasikan rumus matematika di balik setiap efek secara *live*.

---

## Struktur Direktori

```
ETS/
├── main.c                   # Game loop, state machine, input
├── main.exe
├── .vscode/
│   └── tasks.json           # Konfigurasi build VS Code
└── src/
    ├── core/
    │   ├── state.h          # Enum Fase, konstanta SW/SH/PI
    │   └── utils.h / .c     # lerpF, clamp01, MyColorLerp
    ├── algo/
    │   ├── bresenham.h / .c # Algoritma garis Bresenham + ThickLine
    │   ├── dda.h / .c       # Algoritma DDA + FillTriangle
    │   └── midcircle.h / .c # Algoritma Midpoint Circle (outline & filled)
    ├── world/
    │   ├── bintang.h / .c      # Bintang: data, init, update, draw
    │   ├── planet.h / .c    # Planet: data, randomize, update, draw
    │   ├── portal.h / .c    # Portal warp heksagonal
    │   └── kereta.h / .c     # Kereta: roda, connecting rod, body
    └── screens/
        └── menu.h / .c      # Main menu, guide, 4 slide mekanisme
```

---

## Cara Build & Menjalankan

### Prasyarat

| Kebutuhan | Versi |
|---|---|
| [w64devkit](https://github.com/skeeto/w64devkit) | GCC ≥ 13 |
| [raylib](https://www.raylib.com/) | ≥ 5.0 |
| VS Code (opsional) | — |

### Build via VS Code

Buka folder proyek di VS Code, lalu tekan:

```
Ctrl + Shift + B
```

### Build Manual (Command Line)

```bash
cd "path/ke/ETS"

gcc main.c \
    src/core/utils.c \
    src/ui/primitives.c \
    src/world/bintang.c \
    src/world/planet.c \
    src/world/portal.c \
    src/world/kereta.c \
    src/screens/menu.c \
    src/algo/bresenham.c \
    src/algo/dda.c \
    src/algo/midcircle.c \
    -o main.exe -O1 -Wall -Wno-missing-braces \
    -IC:/raylib/raylib/src \
    -LC:/raylib/raylib/src \
    -lraylib -lopengl32 -lgdi32 -lwinmm

./main.exe
```

---

## Kontrol

| Tombol | Fungsi |
|---|---|
| `↑` / `↓` | Navigasi menu |
| `←` / `→` | Ganti slide Mekanisme |
| `ENTER` | Konfirmasi / lanjut |
| `SPACE` | Mulai Warp Jump (dari fase IDLE) |
| `R` | Reset simulasi ke posisi awal |
| `T` | Toggle **Outline Mode** (blueprint wireframe hijau) |
| `H` | Tampilkan / sembunyikan HUD info |
| `BACKSPACE` | Kembali ke menu utama |

---

## Algoritma Grafika yang Diimplementasikan

### 1. Bresenham Line Algorithm (`src/algo/bresenham.c`)

Algoritma rasterisasi garis integer paling efisien. Menghindari operasi floating-point dengan menggunakan **decision variable** berbasis selisih error.

```
Δx = |x2 - x1|,  Δy = |y2 - y1|
p₀ = 2Δy - Δx

Tiap langkah:
  if p < 0  → plot (x+1, y),     p += 2Δy
  if p ≥ 0  → plot (x+1, y+1),   p += 2Δy - 2Δx
```

Digunakan untuk: garis badan kereta, jeruji roda, connecting rod, piston, garis portal, dan bintang streak.

Ekstensi `Bres_ThickLine` menggambar garis tebal dengan cara menggeser titik awal secara tegak lurus sebanyak `thickness/2` piksel, lalu memanggil `BresenhamLine` beberapa kali secara paralel.

---

### 2. DDA (Digital Differential Analyzer) & Triangle Fill (`src/algo/dda.c`)

DDA menggunakan pembagian floating-point untuk menentukan langkah per piksel:

```
steps = max(|Δx|, |Δy|)
xInc  = Δx / steps
yInc  = Δy / steps
```

`FillTriangle` mengisi segitiga dengan metode **scanline**: untuk setiap baris Y antara titik tertinggi dan terendah segitiga, hitung batas kiri dan kanan menggunakan interpolasi DDA, lalu gambar garis horizontal (Bresenham) di antara keduanya.

Digunakan untuk: semua bidang berwarna pada kereta (badan, kepala, jendela, cowcatcher, roda), lapisan isi portal heksagonal.

---

### 3. Midpoint Circle Algorithm (`src/algo/midcircle.c`)

Menggambar lingkaran menggunakan **simetri 8-arah** dengan decision variable integer:

```
p₀ = 1 - r

Tiap langkah (mulai dari titik (0, r)):
  plot 8 titik simetris
  if p < 0  → p += 2x + 3
  if p ≥ 0  → p += 2x - 2y + 5,  y--
  x++
```

`MidcircleFilled` mengisi lingkaran dengan menggambar garis horizontal dari `-x` ke `+x` untuk setiap baris Y yang dihitung.

Digunakan untuk: roda kereta, poros roda, lampu depan, dome, bintang (titik kecil), planet, efek cahaya portal, dan titik singularitas.

---

## Transformasi 2D yang Digunakan

### 1. Translasi

Setiap objek dunia memiliki posisi `(cx, cy)` sebagai titik acuan. Semua titik lokal diterjemahkan ke posisi dunia dengan penambahan vektor:

```
x_world = cx + lx
y_world = cy + ly
```

Digunakan pada: pergerakan kereta (`keretaX`, `keretaY`), pergerakan bintang, pergerakan planet.

---

### 2. Rotasi (2D Rotation Matrix)

Diterapkan pada kereta dan portal. Titik lokal `(lx, ly)` diputar sebesar sudut `angle` terhadap titik pusat `(cx, cy)`:

```
x' = cx + lx·cos(θ) - ly·sin(θ)
y' = cy + lx·sin(θ) + ly·cos(θ)
```

Pada kereta, rotasi kecil (`±0.02 rad`) memberi efek **goyang** saat IDLE. Saat WARP, sudut turun ke `-0.05 rad` untuk kesan condong maju. Pada portal, rotasi terus bertambah tiap frame (`portalRot += dt`) sehingga portal berputar terus-menerus.

---

### 3. Shear (Deformasi Warp)

Ini adalah transformasi paling khas di program ini. Saat warpFactor meningkat, kereta mengalami **shear non-uniform** pada sumbu X dan kompresi pada sumbu Y, meniru efek relativistik length contraction:

```
sx = 1.0 + warpFactor × 3.0   (elongasi horizontal)
sy = 1.0 - warpFactor × 0.3   (kompresi vertikal)

x' = cx + (lx · scale · sx) · cos(θ) - (ly · scale · sy) · sin(θ)
y' = cy + (lx · scale · sx) · sin(θ) + (ly · scale · sy) · cos(θ)
```

Hasilnya: kereta terlihat **memanjang dan menipis** secara proporsional dengan kecepatan warp. Ketika `warpFactor = 1.0`, kereta 4× lebih panjang dari ukuran normalnya.

Efek ini diimplementasikan melalui macro `T(lx, ly)` di `kereta.c` yang menggabungkan rotasi dan scaling non-uniform dalam satu ekspresi.

---

### 4. Scaling (Ukuran Portal)

Portal dibuka secara bertahap dengan parameter `openFactor ∈ [0, 1]`. Semua titik heksagon dikalikan dengan `r × openFactor`:

```
vertex_i = unitPoint_i × (r × openFactor)
```

Saat `openFactor = 0`, portal tidak terlihat. Saat `openFactor = 1`, portal terbuka penuh. Ini adalah **scaling seragam** dari titik pusat portal.

---

### 5. Transformasi Polar → Kartesian (Kinematika Roda)

Posisi jeruji dan pin engkol dihitung dari sudut rotasi menggunakan konversi polar ke kartesian:

```
pin_x = cx + r · cos(θ)
pin_y = cy + r · sin(θ)
```

Setiap roda memiliki kecepatan sudut `ω` yang berbeda berdasarkan rasio jari-jari:

```
ω_kecil = ω_besar × (r_besar / r_kecil)
θ      += -ω · dt   (berlawanan jarum jam)
```

---

### 6. Linear Interpolation / LERP

Digunakan secara ekstensif untuk animasi halus antara dua state:

```
lerp(a, b, t) = a + t × (b - a),   t ∈ [0, 1]
```

Contoh penggunaan:
- Posisi kereta saat WARP: `keretaX = lerp(startX, targetX, ease)`
- Intensitas warp: `warpFactor = lerp(0.3, 1.0, clamp01(t × 1.5))`
- Warna portal: `c = lerp(biru, pink, openFactor)`
- Penutupan portal saat ARRIVAL: `portalOpen = lerp(1.0, 0.0, tKereta)`

Easing quadratic (`ease = t²`) diterapkan pada fase WARP agar kereta terasa **akselerasi** bukan bergerak konstan.

---

### 7. Color Lerp (Gradasi Warna)

`MyColorLerp` menginterpolasi setiap channel RGBA secara independen:

```
R' = R1 + t × (R2 - R1)
G' = G1 + t × (G2 - G1)
B' = B1 + t × (B2 - B1)
A' = A1 + t × (A2 - A1)
```

Digunakan pada portal untuk transisi warna **biru energi → pink/ungu intens** seiring `openFactor` meningkat.

---

## Mekanisme Animasi

### State Machine

Program menggunakan **finite state machine** dengan 10 state:

```
MAIN_MENU → MENU_GUIDE
           → MENU_MEKANISME
           → INPUT_WARP → IDLE → CHARGE → WARP ──→ ARRIVAL → DONE ─┐
                                                  ↘                  │
                                               WARP_LOOP ──────────┘
                                                  ↑_______|  (loop N kali)
```

### Fase CHARGE
Portal kanan mulai membuka (`openFactor` naik dari 0 ke 1 dalam 2 detik). Kereta bergerak maju sedikit dan mulai bergetar lebih cepat.

### Fase WARP
Kereta berakselerasi menuju portal kanan dengan easing `t²`. `warpFactor` naik dari 0.3 ke 1.0, menyebabkan kereta memanjang dan bintang berubah menjadi streak. Saat kepala kereta melewati portal, fase berpindah.

### Fase WARP_LOOP
Untuk lompatan ke-2 dan seterusnya: kereta muncul dari portal kiri dan melesat ke portal kanan dalam 2 detik. Kereta di-**scissor clip** sehingga bagian yang berada di luar kedua portal tidak terlihat, menciptakan ilusi teleportasi.

### Fase ARRIVAL
Portal kiri membuka, kereta keluar darinya dan melambat (easing `1-(1-t)²`) menuju tengah layar. `warpFactor` turun ke 0, kereta kembali ke bentuk normal, kedua portal menutup.

### Scissor Clipping

Teknik GPU `BeginScissorMode` digunakan untuk memotong render kereta sesuai batas portal:

| Fase | Scissor Region |
|---|---|
| WARP | `x ∈ [0, portalX]` — kepala terpotong portal kanan |
| WARP_LOOP | `x ∈ [leftPortalX, portalX]` — diapit dua portal |
| ARRIVAL | `x ∈ [leftPortalX, SW]` — ekor terpotong portal kiri |

### Efek Bintang Streak

Saat `warpFactor > 0.2`, bintang tidak lagi digambar sebagai titik lingkaran melainkan sebagai garis horizontal (`ThickLine`) dengan panjang proporsional terhadap kecepatan dan warpFactor:

```
streak = warpFactor × speed × 0.3
```

### Rotasi Portal Heksagonal Skewed

Portal menggunakan heksagon tidak-beraturan (*skewed*) dengan 6 titik unit asimetris yang diputar setiap frame. Dibuat berlapis (3 lapis transparansi berbeda) untuk efek kedalaman vortex.

---

## Alur State Machine

```
                    ┌─────────────┐
                    │  MAIN_MENU  │
                    └──────┬──────┘
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
     MENU_GUIDE    MENU_MEKANISME    INPUT_WARP
           │               │               │
           └───────────────┘         [Enter angka]
                    │                      │
                    ▼                      ▼
               MAIN_MENU               IDLE
                                          │ [SPACE]
                                          ▼
                                       CHARGE
                                          │ (2.5 detik)
                                          ▼
                                        WARP
                                          │ (kereta lewat portal)
                              ┌───────────┴───────────┐
                              │ count < target         │ count == target
                              ▼                        ▼
                          WARP_LOOP               ARRIVAL
                              │ (loop)                 │ (3.5 detik)
                              │                        ▼
                              └──────────────────────DONE
                                                        │ [Enter]
                                                        ▼
                                                  INPUT_WARP
```

---
