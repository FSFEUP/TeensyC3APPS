#include "can.h"
#include "debug.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t statusRequest;
CAN_message_t status_report;

CAN_message_t torque_request;

CAN_message_t BTB_status;
CAN_message_t BTB_response;

CAN_message_t transmission_request_enable;
CAN_message_t enable_response;

CAN_message_t disable;
CAN_message_t no_disable;
CAN_message_t actualSpeedRequest;

CAN_message_t clear_errors;

CAN_message_t DCVoltageRequest;
CAN_message_t dc_bus_voltage_response;

CAN_message_t request_motor_temp;
CAN_message_t request_current;
CAN_message_t request_powerStage_temp;
CAN_message_t request_rpm;

CAN_message_t Nact_filtered;
CAN_message_t Vout_msg;
CAN_message_t Iq_cmd_msg;
CAN_message_t Iq_actual_msg;
CAN_message_t Mout_msg;
CAN_message_t I_lim_inuse_msg;
CAN_message_t I_actual_filtered_msg;
CAN_message_t Tpeak_msg;
CAN_message_t Imax_peak_msg;
CAN_message_t I_con_eff_msg;

int Nact;
int Vout;
int Iq_cmd;
int Iq_actual;
int Mout;
int I_lim_inuse;
int I_actual_filtered;
int Tpeak;
int Imax_peak;
int I_con_eff;
int Vbat;
int Ibat;

extern elapsedMillis R2DTimer;

extern volatile bool BTBReady;
extern volatile bool transmissionEnabled;
extern volatile bool disabled;
extern volatile bool R2D;
extern volatile bool R2DOverride;

extern int highTemp;
extern int soc;
extern int current;
extern int speedInt;
extern int packVoltage;
extern int lowTemp;
extern int avgTemp;

extern int powerStageTemp;
extern int motorTemp;

extern int rpm;
extern int ACCurrent;

elapsedMillis CAN_timer;
const int CAN_timeout_ms = 100;

#define DC_THRESHOLD 4328

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void initCanMessages() {
    Nact_filtered.id = BAMO_COMMAND_ID;
    Nact_filtered.len = 3;
    Nact_filtered.buf[0] = 0x3D;
    Nact_filtered.buf[1] = 0xA8;
    Nact_filtered.buf[2] = 0x64;

    Vout_msg.id = BAMO_COMMAND_ID;
    Vout_msg.len = 3;
    Vout_msg.buf[0] = 0x3D;
    Vout_msg.buf[1] = 0x8A;
    Vout_msg.buf[2] = 0x64;

    Iq_cmd_msg.id = BAMO_COMMAND_ID;
    Iq_cmd_msg.len = 3;
    Iq_cmd_msg.buf[0] = 0x3D;
    Iq_cmd_msg.buf[1] = 0x26;
    Iq_cmd_msg.buf[2] = 0x64;

    Iq_actual_msg.id = BAMO_COMMAND_ID;
    Iq_actual_msg.len = 3;
    Iq_actual_msg.buf[0] = 0x3D;
    Iq_actual_msg.buf[1] = 0x27;
    Iq_actual_msg.buf[2] = 0x64;

    Mout_msg.id = BAMO_COMMAND_ID;
    Mout_msg.len = 3;
    Mout_msg.buf[0] = 0x3D;
    Mout_msg.buf[1] = 0xA0;
    Mout_msg.buf[2] = 0x64;

    I_lim_inuse_msg.id = BAMO_COMMAND_ID;
    I_lim_inuse_msg.len = 3;
    I_lim_inuse_msg.buf[0] = 0x3D;
    I_lim_inuse_msg.buf[1] = 0x48;
    I_lim_inuse_msg.buf[2] = 0x64;

    I_actual_filtered_msg.id = BAMO_COMMAND_ID;
    I_actual_filtered_msg.len = 3;
    I_actual_filtered_msg.buf[0] = 0x3D;
    I_actual_filtered_msg.buf[1] = 0x5F;
    I_actual_filtered_msg.buf[2] = 0x64;

    Tpeak_msg.id = BAMO_COMMAND_ID;
    Tpeak_msg.len = 3;
    Tpeak_msg.buf[0] = 0x3D;
    Tpeak_msg.buf[1] = 0xF0;
    Tpeak_msg.buf[2] = 0x64;

    Imax_peak_msg.id = BAMO_COMMAND_ID;
    Imax_peak_msg.len = 3;
    Imax_peak_msg.buf[0] = 0x3D;
    Imax_peak_msg.buf[1] = 0xC4;
    Imax_peak_msg.buf[2] = 0x64;

    I_con_eff_msg.id = BAMO_COMMAND_ID;
    I_con_eff_msg.len = 3;
    I_con_eff_msg.buf[0] = 0x3D;
    I_con_eff_msg.buf[1] = 0xC5;
    I_con_eff_msg.buf[2] = 0x64;

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

    statusRequest.id = BAMO_COMMAND_ID;
    statusRequest.len = 3;
    statusRequest.buf[0] = 0x3D;
    statusRequest.buf[1] = 0x40;
    statusRequest.buf[2] = 0x00;

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

    actualSpeedRequest.id = BAMO_COMMAND_ID;
    actualSpeedRequest.len = 3;
    actualSpeedRequest.buf[0] = 0x3D;
    actualSpeedRequest.buf[1] = 0x30;
    actualSpeedRequest.buf[2] = 0x64;

    DCVoltageRequest.id = BAMO_COMMAND_ID;
    DCVoltageRequest.len = 3;
    DCVoltageRequest.buf[0] = 0x3D;
    DCVoltageRequest.buf[1] = 0xEB;
    DCVoltageRequest.buf[2] = 0x64;

    dc_bus_voltage_response.id = BAMO_RESPONSE_ID;
    dc_bus_voltage_response.len = 4;
    dc_bus_voltage_response.buf[0] = 0xEB;
}

void sendMsg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    torque_request.buf[1] = byte2;
    torque_request.buf[2] = byte1;

    can1.write(torque_request);
}

void initBamocarD3() {
    can1.write(clear_errors);

    while (not transmissionEnabled and CAN_timer > CAN_timeout_ms) {
        can1.write(transmission_request_enable);
        CAN_timer = 0;
    }

    while (not BTBReady and CAN_timer > CAN_timeout_ms) {
        can1.write(BTB_status);
        CAN_timer = 0;
    }

    can1.write(no_disable);
}

void REGIDHandler(const CAN_message_t& msg) {
    switch (msg.buf[0]) {
        case REGID_NACT:
            Nact = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_VOUT:
            Vout = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_ACTUAL_IQ:
            Iq_actual = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_CMD_IQ:
            Iq_cmd = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_MOUT:
            Mout = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_LIM_INUSE:
            I_lim_inuse = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_ACT_FILTERED:
            I_actual_filtered = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_T_PEAK:
            Tpeak = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_MAX_PEAK:
            Imax_peak = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_CON_EFF:
            I_con_eff = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_ACTUAL_SPEED: {
            double speed = 0;
            speed = (msg.buf[2] << 8) | msg.buf[1];
            if (speed < 0)
                speed *= -1;
            rpm = speed;
            rpm = (rpm * 6500) / 32760;
            speed = (speed / 5.04) * 0.02394;
            speedInt = (int)speed;
            break;
        }

        case REGID_DC_VOLTAGE: {
            long dc_voltage = 0;
            dc_voltage = (msg.buf[2] << 8) | msg.buf[1];
            R2D = (dc_voltage >= DC_THRESHOLD);
            break;
        }

        case REGID_IGBT:
            powerStageTemp = (msg.buf[2] << 8) | msg.buf[1];
            powerStageTemp = (int)(powerStageTemp / 103.969 - 158.29);
            break;

        case REGID_AC_CURRENT:
            ACCurrent = (msg.buf[2] << 8) | msg.buf[1];
            ACCurrent = (ACCurrent * MAX_I) / ADC_MAX;
            break;

        case REGID_MOTOR_TEMP:
            motorTemp = (msg.buf[2] << 8) | msg.buf[1];
            motorTemp = motorTemp * 0.0194 - 160;
            break;

        default:
            break;
    }
}

void canSniffer(const CAN_message_t& msg) {
#ifdef CAN_DEBUG
    LOG("CAN message received\n");
    INFO("Message ID: %x\n", msg.id);
    INFO("Message length: %d\n", msg.len);
    INFO("Message data: ");
    for (int i = 0; i < msg.len; i++)
        Serial.printf("%x ", msg.buf[i]);
    Serial.println();
#endif  // CAN_DEBUG

    switch (msg.id) {
        case C3_ID:
            R2DTimer = 0;
            break;

        case R2D_ID:
            R2DOverride = true;
            break;

        case BAMO_RESPONSE_ID:
            if (msg.len == 4) {
                BTBReady = (msg.buf[0] == BTB_response.buf[0] and msg.buf[1] == BTB_response.buf[1] and msg.buf[2] == BTB_response.buf[2] and msg.buf[3] == BTB_response.buf[3]);
                if (BTBReady)
                    Serial.println("BTB ready");
                break;
            }
            if (msg.len == 3) {
                transmissionEnabled = (msg.buf[0] == enable_response.buf[0] and msg.buf[1] == enable_response.buf[1] and msg.buf[2] == enable_response.buf[2]);
                if (transmissionEnabled)
                    Serial.println("Transmission enabled");
                break;
            }

            break;
        case BMS_ID:
            current = ((msg.buf[1] << 8) | msg.buf[0]) / 10;
            Ibat = current;
            soc = msg.buf[2] / 2;
            lowTemp = msg.buf[3];
            highTemp = msg.buf[4];
            packVoltage = ((msg.buf[6] << 8) | msg.buf[5]) / 10;
            Vbat = packVoltage;
            //avgTemp = msg.buf[7];
            break;

        default:
            break;
    }

    REGIDHandler(msg);
}

void canSetup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, C3_ID, STD);
    can1.setFIFOFilter(1, BAMO_RESPONSE_ID, STD);
    can1.setFIFOFilter(2, R2D_ID, STD);
    can1.setFIFOFilter(3, BMS_ID, STD);
    can1.onReceive(canSniffer);

    initCanMessages();
}