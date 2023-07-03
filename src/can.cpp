#include "can.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t request_bamo;
CAN_message_t bamo_apps;
CAN_message_t BTB;
CAN_message_t BTB_response;
CAN_message_t disable;
CAN_message_t transmission_request_enable;
CAN_message_t enable_response;
CAN_message_t no_disable;
CAN_message_t request_actual_speed;

extern elapsedMillis r2d_timer;

extern volatile bool BTB_ready;
extern volatile bool transmission_enabled;
extern volatile bool disabled;
extern volatile bool r2d_override;

extern int tempInt;
extern int socInt;
extern int current;
extern int speedInt;

elapsedMillis CAN_timer;
const int CAN_timeout_ms = 100;

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void init_can_messages() {
    // Message 1
    request_bamo.id = BAMO_RESPONSE_ID;
    request_bamo.len = 3;
    request_bamo.buf[0] = 0x30;
    request_bamo.buf[1] = 0x8F;
    request_bamo.buf[2] = 0x00;

    BTB.id = BAMO_COMMAND_ID;
    BTB.len = 3;
    BTB.buf[0] = 0x3D;
    BTB.buf[1] = 0xE2;
    BTB.buf[2] = 0x00;

    BTB_response.id = BAMO_RESPONSE_ID;
    BTB_response.len = 4;
    BTB_response.buf[0] = 0xE2;
    BTB_response.buf[1] = 0x01;
    BTB_response.buf[2] = 0x00;
    BTB_response.buf[3] = 0x00;

    disable.id = BAMO_COMMAND_ID;
    disable.len = 3;
    disable.buf[0] = 0x51;
    disable.buf[1] = 0x04;
    disable.buf[2] = 0x00;

    transmission_request_enable.id = BAMO_COMMAND_ID;
    transmission_request_enable.len = 3;
    transmission_request_enable.buf[0] = 0x3D;
    transmission_request_enable.buf[1] = 0xE8;
    transmission_request_enable.buf[2] = 0x00;

    enable_response.id = BAMO_RESPONSE_ID;
    enable_response.len = 4;
    enable_response.buf[0] = 0xE8;
    enable_response.buf[1] = 0x01;
    enable_response.buf[2] = 0x00;
    enable_response.buf[3] = 0x00;

    no_disable.id = BAMO_COMMAND_ID;
    no_disable.len = 3;
    no_disable.buf[0] = 0x51;
    no_disable.buf[1] = 0x00;
    no_disable.buf[2] = 0x00;

    bamo_apps.id = BAMO_COMMAND_ID;
    bamo_apps.len = 3;
    bamo_apps.buf[0] = 0x90;

    request_actual_speed.id = BAMO_COMMAND_ID;
    request_actual_speed.len = 3;
    request_actual_speed.buf[0] = 0x3D;
    request_actual_speed.buf[1] = 0x30;
    request_actual_speed.buf[2] = 0x64;
}

void send_msg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    bamo_apps.buf[1] = byte2;
    bamo_apps.buf[2] = byte1;

    can1.write(bamo_apps);
}

void BAMO_init_operation() {
    while (not BTB_ready and CAN_timer > CAN_timeout_ms) {
        can1.write(BTB);
        CAN_timer = 0;
    }

    while (not transmission_enabled and CAN_timer > CAN_timeout_ms) {
        can1.write(disable);
        can1.write(transmission_request_enable);
        CAN_timer = 0;
    }

    can1.write(no_disable);
}

void canbus_listener(const CAN_message_t& msg) {
    Serial.println("CAN message received");
    Serial.print("Message ID: ");
    Serial.println(msg.id, HEX);
    switch (msg.id) {
        case R2D_ID:
            BAMO_init_operation();
            r2d_override = true;
        case BAMO_RESPONSE_ID:
            if (msg.len == 4) {
                BTB_ready = (msg.buf[0] == BTB_response.buf[0] and msg.buf[1] == BTB_response.buf[1] and msg.buf[2] == BTB_response.buf[2] and msg.buf[3] == BTB_response.buf[3]);
                if (BTB_ready) Serial.println("BTB ready");
                else if (msg.buf[0] == 0x30) {
                    speedInt = msg.buf[1] + msg.buf[2];
                }

            }
            break;
            if (msg.len == 3) {
                transmission_enabled = (msg.buf[0] == enable_response.buf[0] and msg.buf[1] == enable_response.buf[1] and msg.buf[2] == enable_response.buf[2]);
                if (transmission_enabled) Serial.println("Transmission enabled");
            }
            break;
        case BMS_ID:
            current = msg.buf[0];
            socInt = msg.buf[1] + msg.buf[2];
            tempInt = msg.buf[3];
        default:
            break;
    }
}

void canbus_setup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, C3_ID, STD);
    can1.setFIFOFilter(1, BAMO_RESPONSE_ID, STD);
    can1.setFIFOFilter(2, R2D_ID, STD);
    can1.setFIFOFilter(3, BMS_ID, STD);
    can1.onReceive(canbus_listener);

    init_can_messages();
}