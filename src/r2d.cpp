#include "r2d.h"

int check_bamocar() {
    return 0;
}

int send_to_bamocar() {
    return 0;
}

int check_BMS() {
    return 0;
}

int play_r2d_sound() {
    return 0;
}

void r2d_state_update(r2d_mode* state) {
    return;
}

r2d_mode r2d_state_machine(r2d_mode cur_state) {
    r2d_mode next_state = cur_state;
    switch (cur_state) {
        case R2D_MODE_STARTUP:
            // delay(POWER_ON_DELAY_MS);
            next_state = R2D_MODE_IDLE;
            break;
        case R2D_MODE_IDLE:
            // check bamo
            // check bms
            // check apps
            // check modo dash
            // check modo volante
            // check fim precharge
            // check r2d button
            // check brake
            // update display
            break;
        case R2D_MODE_DRIVE:

            break;
        case R2D_MODE_ERROR:
            break;

        default:
            next_state = R2D_MODE_ERROR;
            break;
    }
    return next_state;
}