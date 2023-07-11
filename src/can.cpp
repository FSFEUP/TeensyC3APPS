#include "can.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t status_request;
CAN_message_t status_report;

CAN_message_t torque_request;

CAN_message_t BTB_status;
CAN_message_t BTB_response;

CAN_message_t transmission_request_enable;
CAN_message_t enable_response;

CAN_message_t disable;
CAN_message_t no_disable;
CAN_message_t request_actual_speed;

CAN_message_t clear_errors;

CAN_message_t dc_bus_voltage_request;
CAN_message_t dc_bus_voltage_response;

CAN_message_t request_motor_temp;
CAN_message_t request_current;
CAN_message_t request_powerStage_temp;
CAN_message_t request_rpm;

extern elapsedMillis r2d_timer;

extern volatile bool BTB_ready;
extern volatile bool transmission_enabled;
extern volatile bool disabled;
extern volatile bool r2d;

extern int high_tempInt;
extern int socInt;
extern int current;
extern int speedInt;
extern int pack_voltage;
extern int low_tempInt;

extern int power_stage_temp;
extern int motor_temp;

extern int rpm;
extern int ac_current;

elapsedMillis CAN_timer;
const int CAN_timeout_ms = 100;

#define DC_THRESHOLD 4328

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void init_can_messages() {
    request_rpm.id = BAMO_COMMAND_ID;
    request_rpm.len = 3;
    request_rpm.buf[0] = 0x3D;
    request_rpm.buf[1] = 0xCE;
    request_rpm.buf[2] = 0x64;

    request_powerStage_temp.id = BAMO_COMMAND_ID;
    request_powerStage_temp.len = 3;
    request_powerStage_temp.buf[0] = 0x3D;
    request_powerStage_temp.buf[1] = 0x4A;
    request_powerStage_temp.buf[0] = 0x64;

    request_current.id = BAMO_COMMAND_ID;
    request_current.len = 3;
    request_current.buf[0] = 0x3D;
    request_current.buf[1] = 0x20;
    request_current.buf[2] = 0x64;

    request_motor_temp.id = BAMO_COMMAND_ID;
    request_motor_temp.len = 3;
    request_motor_temp.buf[0] = 0x3D;
    request_motor_temp.buf[1] = 0x49;
    request_motor_temp.buf[2] = 0x64;

    status_request.id = BAMO_COMMAND_ID;
    status_request.len = 3;
    status_request.buf[0] = 0x3D;
    status_request.buf[1] = 0x40;
    status_request.buf[2] = 0x00;

    status_report.id = BAMO_RESPONSE_ID;
    status_report.len = 3;
    status_report.buf[0] = 0x40;

    BTB_status.id = BAMO_COMMAND_ID;
    BTB_status.len = 3;
    BTB_status.buf[0] = 0x3D;
    BTB_status.buf[1] = 0xE2;
    BTB_status.buf[2] = 0x00;

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
    clear_errors.buf[0] = 0x8E;
    clear_errors.buf[1] = 0x44;
    clear_errors.buf[2] = 0x4D;

    request_actual_speed.id = BAMO_COMMAND_ID;
    request_actual_speed.len = 3;
    request_actual_speed.buf[0] = 0x3D;
    request_actual_speed.buf[1] = 0x30;
    request_actual_speed.buf[2] = 0x64;

    dc_bus_voltage_request.id = BAMO_COMMAND_ID;
    dc_bus_voltage_request.len = 3;
    dc_bus_voltage_request.buf[0] = 0x3D;
    dc_bus_voltage_request.buf[1] = 0xEB;
    dc_bus_voltage_request.buf[2] = 0x64;

    dc_bus_voltage_response.id = BAMO_RESPONSE_ID;
    dc_bus_voltage_response.len = 4;
    dc_bus_voltage_response.buf[0] = 0xEB;
}

void send_msg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    torque_request.buf[1] = byte2;
    torque_request.buf[2] = byte1;

    can1.write(torque_request);
}

void BAMO_init_operation() {
    can1.write(clear_errors);

    while (not transmission_enabled and CAN_timer > CAN_timeout_ms) {
        can1.write(transmission_request_enable);
        CAN_timer = 0;
    }

    while (not BTB_ready and CAN_timer > CAN_timeout_ms) {
        can1.write(BTB_status);
        CAN_timer = 0;
    }

    can1.write(no_disable);
}

void canbus_listener(const CAN_message_t& msg) {
    // Serial.println("CAN message received");
    // Serial.print("Message ID: ");
    // Serial.println(msg.id, HEX);
    // Serial.print("Message length: ");
    // Serial.println(msg.len);
    // Serial.print("Message data: ");
    // for (int i = 0; i < msg.len; i++) {
    //     Serial.print(msg.buf[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();
    switch (msg.id) {
        case C3_ID:
            r2d_timer = 0;
            break;
        case R2D_ID:
            BAMO_init_operation();

            r2d = true;
        case BAMO_RESPONSE_ID:
            if (msg.len == 4) {
                double speed = 0;
                long dc_voltage = 0;
                switch (msg.buf[0]) {
                    case 0x30:  // speed
                        speed = (msg.buf[2] << 8) | msg.buf[1];
                        speed = speed / 5.04;
                        speed = speed * 0.02394;
                        speedInt = (int)speed;
                        break;
                    case 0xEB:  // dc bus voltage
                        dc_voltage = (msg.buf[2] << 8) | msg.buf[1];
                        r2d = (dc_voltage >= DC_THRESHOLD);
                        break;
                    case 0x4A:
                        power_stage_temp = (msg.buf[2] << 8) | msg.buf[1];
                        power_stage_temp = (int)(power_stage_temp / 103.969 - 158.29);
                        break;
                    case 0xCE:
                        rpm = (msg.buf[2] << 8) | msg.buf[1];
                        break;
                    case 0x20:
                        ac_current = (msg.buf[2] << 8) | msg.buf[1];
                        break;
                    case 0x49:
                        motor_temp = (msg.buf[2] << 8) | msg.buf[1];
                        break;
                    default:
                        break;
                }

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
        case BMS_ID:
            current = ((msg.buf[1] << 8) | msg.buf[0]) / 10;
            socInt = msg.buf[2] / 2;
            low_tempInt = msg.buf[3];
            high_tempInt = msg.buf[4];
            pack_voltage = ((msg.buf[6] << 8) | msg.buf[5]) / 10;
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