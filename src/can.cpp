#include "can.h"
#include "debug.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t disable;
CAN_message_t BTBStatus;
CAN_message_t noDisable;
CAN_message_t clearErrors;
CAN_message_t BTBResponse;
CAN_message_t statusRequest;
CAN_message_t torqueRequest;
CAN_message_t enableResponse;
CAN_message_t DCVoltageRequest;
CAN_message_t actualSpeedRequest;
CAN_message_t transmissionRequestEnable;

#if DATA_DISPLAY > 0

CAN_message_t DCVoltageResponse;

CAN_message_t motorTempRequest;
CAN_message_t currentRequest;
CAN_message_t powerStageTempRequest;
CAN_message_t rpmRequest;

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
#endif

int Ibat;
int Vbat;
int Mout;
int Nact;
int Vout;
int Tpeak;
int Iq_cmd;
int Imax_peak;
int I_con_eff;
int Iq_actual;
int I_lim_inuse;
int I_actual_filtered;

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

extern int powerStageTemp;
extern int motorTemp;

extern int rpm;
extern int ACCurrent;

extern elapsedMillis R2DTimer;

elapsedMillis CANTimer;
const int CANTimeoutMS = 100;

// #define DC_THRESHOLD 4328
#define DC_THRESHOLD 500

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void initCanMessages() {
#if DATA_DISPLAY > 1
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

    rpmRequest.id = BAMO_COMMAND_ID;
    rpmRequest.len = 3;
    rpmRequest.buf[0] = 0x3D;
    rpmRequest.buf[1] = 0xCE;
    rpmRequest.buf[2] = 0x64;

    powerStageTempRequest.id = BAMO_COMMAND_ID;
    powerStageTempRequest.len = 3;
    powerStageTempRequest.buf[0] = 0x3D;
    powerStageTempRequest.buf[1] = 0x4A;
    powerStageTempRequest.buf[0] = 0x64;

    currentRequest.id = BAMO_COMMAND_ID;
    currentRequest.len = 3;
    currentRequest.buf[0] = 0x3D;
    currentRequest.buf[1] = 0x20;
    currentRequest.buf[2] = 0x64;

    motorTempRequest.id = BAMO_COMMAND_ID;
    motorTempRequest.len = 3;
    motorTempRequest.buf[0] = 0x3D;
    motorTempRequest.buf[1] = 0x49;
    motorTempRequest.buf[2] = 0x64;

    DCVoltageResponse.id = BAMO_RESPONSE_ID;
    DCVoltageResponse.len = 4;
    DCVoltageResponse.buf[0] = 0xEB;
#endif
    // APPS Message
    torqueRequest.id = BAMO_COMMAND_ID;
    torqueRequest.len = 3;
    torqueRequest.buf[0] = 0x90;

    enableResponse.id = BAMO_RESPONSE_ID;
    enableResponse.len = 4;
    enableResponse.buf[0] = 0xE8;
    enableResponse.buf[1] = 0x01;
    enableResponse.buf[2] = 0x00;
    enableResponse.buf[3] = 0x00;

    clearErrors.id = BAMO_COMMAND_ID;
    clearErrors.len = 3;
    clearErrors.buf[0] = 0x8E;
    clearErrors.buf[1] = 0x44;
    clearErrors.buf[2] = 0x4D;

    noDisable.id = BAMO_COMMAND_ID;
    noDisable.len = 3;
    noDisable.buf[0] = 0x51;
    noDisable.buf[1] = 0x00;
    noDisable.buf[2] = 0x00;

    BTBStatus.id = BAMO_COMMAND_ID;
    BTBStatus.len = 3;
    BTBStatus.buf[0] = 0x3D;
    BTBStatus.buf[1] = 0xE2;
    BTBStatus.buf[2] = 0x00;

    BTBResponse.id = BAMO_RESPONSE_ID;
    BTBResponse.len = 4;
    BTBResponse.buf[0] = 0xE2;
    BTBResponse.buf[1] = 0x01;
    BTBResponse.buf[2] = 0x00;
    BTBResponse.buf[3] = 0x00;

    transmissionRequestEnable.id = BAMO_COMMAND_ID;
    transmissionRequestEnable.len = 3;
    transmissionRequestEnable.buf[0] = 0x3D;
    transmissionRequestEnable.buf[1] = 0xE8;
    transmissionRequestEnable.buf[2] = 0x00;

    statusRequest.id = BAMO_COMMAND_ID;
    statusRequest.len = 3;
    statusRequest.buf[0] = 0x3D;
    statusRequest.buf[1] = 0x40;
    statusRequest.buf[2] = 0x00;

    disable.id = BAMO_COMMAND_ID;
    disable.len = 3;
    disable.buf[0] = 0x51;
    disable.buf[1] = 0x04;
    disable.buf[2] = 0x00;

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
}

void sendMsg(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    torqueRequest.buf[1] = byte2;
    torqueRequest.buf[2] = byte1;

    can1.write(torqueRequest);
}

void initBamocarD3() {
    can1.write(clearErrors);

    while (not transmissionEnabled and CANTimer > CANTimeoutMS) {
        can1.write(transmissionRequestEnable);
        CANTimer = 0;
    }

    while (not BTBReady and CANTimer > CANTimeoutMS) {
        can1.write(BTBStatus);
        CANTimer = 0;
    }

    can1.write(noDisable);
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
#ifdef CAN_DEBUG
            LOG("DC Voltage: %d\n", dc_voltage);
#endif
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
#ifdef R2D_DEBUG
            INFO("Braking signal received\n");
#endif  // R2D_DEBUG
            R2DTimer = 0;
            break;

        case R2D_ID:
            R2DOverride = true;
            break;

        case BAMO_RESPONSE_ID:
            if (msg.len == 4) {
                BTBReady = (msg.buf[0] == BTBResponse.buf[0] and msg.buf[1] == BTBResponse.buf[1] and msg.buf[2] == BTBResponse.buf[2] and msg.buf[3] == BTBResponse.buf[3]);
                if (BTBReady)
                    Serial.println("BTB ready");
                else
                    REGIDHandler(msg);
                break;
            }
            if (msg.len == 3) {
                transmissionEnabled = (msg.buf[0] == enableResponse.buf[0] and msg.buf[1] == enableResponse.buf[1] and msg.buf[2] == enableResponse.buf[2]);
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
            break;

        default:
            break;
    }
}

void canSetup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, C3_ID, STD);
    can1.setFIFOFilter(1, R2D_ID, STD);
    can1.setFIFOFilter(2, BMS_ID, STD);
    can1.setFIFOFilter(3, BAMO_RESPONSE_ID, STD);
    can1.onReceive(canSniffer);

    initCanMessages();
}