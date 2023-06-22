#ifndef _R2D_H_
#define _R2D_H_

#define POWER_ON_DELAY_MS 15000

typedef enum _r2d_mode {
    R2D_MODE_STARTUP,
    R2D_MODE_IDLE,
    R2D_MODE_DRIVE,
    R2D_MODE_ERROR
} r2d_mode;

int send_to_bamocar(int value_bamo);
void can_setup();

#endif  // _R2D_H_