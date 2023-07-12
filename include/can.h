#ifndef _CAN_H_
#define _CAN_H_

#include <FlexCAN_T4.h>

#define BAMO_RESPONSE_ID 0x181
#define BAMO_COMMAND_ID 0x201
#define C3_ID 0x123
#define R2D_ID 0x666
#define BMS_ID 0x675
#define regID_ACTUAL_SPEED 0x30
#define regID_dc_bus_voltage 0xEB
#define regID_igbt 0x4A
#define regID_ac_Current 0x20
#define regID_motor_temp 0x49
#define ADC_max 65536
#define max_I 250
#define Nact_regID 0xA8
#define Vout_regID 0x8A
#define Iq_actual_regID 0x27
#define Iq_cmd_regID 0x26
#define Mout_regID 0xA0
#define I_lim_inuse_regID 0x48
#define I_act_filtered_regID 0x5F
#define Tpeak_regID 0xF0
#define Imax_peak_regID 0xC4
#define I_con_eff_regID 0xC5

void canbus_setup();
void canbus_listener(const CAN_message_t& msg);
void send_msg(int value_bamo);
void init_can_messages();
void BAMO_init_operation();
#endif