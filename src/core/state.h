#ifndef STATE_H
#define STATE_H

#define SW 1920
#define SH 1080

#ifndef PI
#define PI 3.14159f
#endif

typedef enum {
    MAIN_MENU,
    MENU_GUIDE,
    MENU_MEKANISME,
    INPUT_WARP,
    IDLE,
    CHARGE,
    WARP,
    WARP_LOOP,
    ARRIVAL,
    DONE
} Fase;

#endif