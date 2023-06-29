#ifndef CAN_MESSAGES_H
#define CAN_MESSAGES_H

#include <FlexCAN_T4.h>

#define MESSAGE_1_ID 0x181
#define MESSAGE_2_ID 0x201

extern CAN_message_t request_bamo;
extern CAN_message_t bamo_apps;

// Initialize CAN messages
void initCanMessages() {
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

#endif // CAN_MESSAGES_H
