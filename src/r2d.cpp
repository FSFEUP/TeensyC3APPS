#include "r2d.h"

#include <Arduino.h>
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

int check_bamocar() {
    return 0;
}

void can_setup() {
    can1.begin();
    can1.setBaudRate(500000);
}

int send_to_bamocar(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    CAN_message_t msg;
    msg.id = 0x201;
    msg.len = 3;
    msg.buf[0] = 0x31;
    msg.buf[1] = byte2;
    msg.buf[2] = byte1;

    can1.write(msg);
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