#include "can.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t request_bamo;
CAN_message_t bamo_apps;
CAN_message_t msg_transmissionRequest_BTB;
CAN_message_t receiving_BTB;
CAN_message_t transmitting_disable;
CAN_message_t transmitting_request_enable;
CAN_message_t receiving_enable;
CAN_message_t transmitting_enable;
CAN_message_t transmitting_ACC_ramp;
CAN_message_t transmitting_DEC_ramp;

extern elapsedMillis r2d_timer;

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void init_can_messages() {
    // Message 1
    request_bamo.id = MESSAGE_1_ID;
    request_bamo.len = 3;
    request_bamo.buf[0] = 0x30;
    request_bamo.buf[1] = 0x8F;
    request_bamo.buf[2] = 0x00;

    msg_transmissionRequest_BTB.id = MESSAGE_2_ID;
    msg_transmissionRequest_BTB.len = 3;
    msg_transmissionRequest_BTB.buf[0] = 0x3D;
    msg_transmissionRequest_BTB.buf[1] = 0xE2;
    msg_transmissionRequest_BTB.buf[2] = 0x00;

    receiving_BTB.id = MESSAGE_1_ID;
    receiving_BTB.len = 4;
    receiving_BTB.buf[0] = 0xE2;
    receiving_BTB.buf[1] = 0x01;
    receiving_BTB.buf[2] = 0x00;
    receiving_BTB.buf[3] = 0x00;

    transmitting_disable.id = MESSAGE_2_ID;
    transmitting_disable.len = 3;
    transmitting_disable.buf[0] = 0x51;
    transmitting_disable.buf[1] = 0x04;
    transmitting_disable.buf[2] = 0x00;

    transmitting_request_enable.id = MESSAGE_2_ID;
    transmitting_request_enable.len = 3;
    transmitting_request_enable.buf[0] = 0x3D;
    transmitting_request_enable.buf[1] = 0xE8;
    transmitting_request_enable.buf[2] = 0x00;

    receiving_enable.id = MESSAGE_1_ID;
    receiving_enable.len = 4;
    receiving_enable.buf[0] = 0xE8;
    receiving_enable.buf[1] = 0x01;
    receiving_enable.buf[2] = 0x00;
    receiving_enable.buf[3] = 0x00;

    transmitting_enable.id = MESSAGE_2_ID;
    transmitting_enable.len = 3;
    transmitting_enable.buf[0] = 0x51;
    transmitting_enable.buf[1] = 0x00;
    transmitting_enable.buf[2] = 0x00;

    transmitting_ACC_ramp.id = MESSAGE_2_ID;
    transmitting_ACC_ramp.len = 3;
    transmitting_ACC_ramp.buf[0] = 0x35;
    transmitting_ACC_ramp.buf[1] = 0xF4;  // 500ms
    transmitting_ACC_ramp.buf[2] = 0x01;

    transmitting_DEC_ramp.id = MESSAGE_2_ID;
    transmitting_DEC_ramp.len = 3;
    transmitting_DEC_ramp.buf[0] = 0xED;
    transmitting_DEC_ramp.buf[1] = 0xE8;  // 1000ms
    transmitting_DEC_ramp.buf[2] = 0x03;

    bamo_apps.id = MESSAGE_2_ID;
    bamo_apps.len = 3;
    bamo_apps.buf[0] = 0x90;
}

void send_msg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    bamo_apps.buf[1] = byte2;
    bamo_apps.buf[2] = byte1;

    can1.write(bamo_apps);
}

void canbus_listener(const CAN_message_t& msg) {
    Serial.println("CAN message received");
    Serial.print("Message ID: ");
    Serial.println(msg.id, HEX);
    r2d_timer = 0;
}

void canbus_setup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, 0x123, STD);
    can1.onReceive(canbus_listener);

    init_can_messages();
}