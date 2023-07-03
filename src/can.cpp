#include "can.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t status_request;
CAN_message_t status_report;

CAN_message_t torque_request;

CAN_message_t BTB;
CAN_message_t BTB_response;

CAN_message_t transmission_request_enable;
CAN_message_t enable_response;

CAN_message_t disable;
CAN_message_t no_disable;

CAN_message_t clear_errors;

extern elapsedMillis r2d_timer;

extern volatile bool BTB_ready;
extern volatile bool transmission_enabled;
extern volatile bool disabled;
extern volatile bool r2d_override;

elapsedMillis CAN_timer;
const int CAN_timeout_ms = 100;

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void init_can_messages() {
    status_request.id = BAMO_COMMAND_ID;
    status_request.len = 3;
    status_request.buf[0] = 0x3D;
    status_request.buf[1] = 0x40;
    status_request.buf[2] = 0x00;

    status_report.id = BAMO_RESPONSE_ID;
    status_report.len = 3;
    status_report.buf[0] = 0x40;

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

    torque_request.id = BAMO_COMMAND_ID;
    torque_request.len = 3;
    torque_request.buf[0] = 0x90;

    clear_errors.id = BAMO_COMMAND_ID;
    clear_errors.len = 3;
    clear_errors.buf[0] = 0x3D; 
}

void send_msg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    torque_request.buf[1] = byte2;
    torque_request.buf[2] = byte1;

    can1.write(torque_request);
}

void BAMO_init_operation() {
    while (not BTB_ready and CAN_timer > CAN_timeout_ms) {
        can1.write(BTB);
        CAN_timer = 0;
    }

    while (not transmission_enabled and CAN_timer > CAN_timeout_ms) {
        can1.write(transmission_request_enable);
        CAN_timer = 0;
    }

    can1.write(no_disable);
}

void canbus_listener(const CAN_message_t& msg) {
    Serial.println("CAN message received");
    Serial.print("Message ID: ");
    Serial.println(msg.id, HEX);
    Serial.print("Message length: ");
    Serial.println(msg.len);
    Serial.print("Message data: ");
    for (int i = 0; i < msg.len; i++) {
        Serial.print(msg.buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    switch (msg.id) {
        case C3_ID:
            r2d_timer = 0;
            break;
        case R2D_ID:
            BAMO_init_operation();
            r2d_override = true;
        case BAMO_RESPONSE_ID:
            if (msg.len == 4) {
                BTB_ready = (msg.buf[0] == BTB_response.buf[0] and msg.buf[1] == BTB_response.buf[1] and msg.buf[2] == BTB_response.buf[2] and msg.buf[3] == BTB_response.buf[3]);
                if (BTB_ready)
                    Serial.println("BTB ready");
            }
            break;
            if (msg.len == 3) {
                transmission_enabled = (msg.buf[0] == enable_response.buf[0] and msg.buf[1] == enable_response.buf[1] and msg.buf[2] == enable_response.buf[2]);
                if (transmission_enabled)
                    Serial.println("Transmission enabled");
            }
            break;
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

    can1.onReceive(canbus_listener);

    init_can_messages();
}