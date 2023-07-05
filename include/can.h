#ifndef _CAN_H_
#define _CAN_H_

#include <FlexCAN_T4.h>

#define BAMO_RESPONSE_ID 0x181
#define BAMO_COMMAND_ID 0x201
#define C3_ID 0x123
#define R2D_ID 0x666
#define BMS_ID 0x675

void canbus_setup();
void canbus_listener(const CAN_message_t& msg);
void send_msg(int value_bamo);
void init_can_messages();
void BAMO_init_operation();
#endif