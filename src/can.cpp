#include "can.h"


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t request_bamo;
CAN_message_t bamo_apps;

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

    // Message 2
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