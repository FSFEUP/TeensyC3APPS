#ifndef _CAN_H_
#define _CAN_H_

#include <FlexCAN_T4.h>

#define MESSAGE_1_ID 0x181
#define MESSAGE_2_ID 0x201

void canbus_setup();
void canbus_listener(const CAN_message_t& msg);
void send_msg(int value_bamo);
void init_can_messages();

#endif