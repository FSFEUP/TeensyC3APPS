#include "display.h"
#include "apps.h"

#include <EasyNextionLibrary.h>
#include <FlexCAN_T4.h>
#include <vector>

#define NEXTION_PORT Serial1

EasyNex myNex(NEXTION_PORT);

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

int switchPin = 14;
int dashPin = 26;

int rpm = 0;
int soc = 0;
int current = 0;
int speedInt = 0;
int ACCurrent = 0;
int motorTemp = 0;
int lowTemp = 0;
int sensorValue = 0;
int highTemp = 0;
int currentPage = 0;
int packVoltage = 0;
int powerStageTemp = 0;

int actual_mode_wheel;
int actual_mode_dash;

std::vector<int> N_lim_values = {4915, 10813, 32767, 19005, 32767, 19005,0,0,0,0,0,0};
std::vector<int> i_con_eff_values = {2088, 14909, 14909, 14909, 14909, 14909,0,0,0,0,0,0};
std::vector<int> i_max_peak_values = {836, 5598, 5598, 5598, 5598, 5598,0,0,0,0,0,0};

/*
std::vector<int> ACramp_values = {5470, 10939, 16409, 21879, 27348,32818, 38288, 43758, 49227, 54697, 60167, 65636};

void set_modes_ACramp(int pos){
    uint8_t byte1 = (ACramp_values[pos] >> 8) & 0xFF;
    uint8_t byte2 = ACramp_values[pos] & 0xFF;

    CAN_message_t change_ACramp;

    change_ACramp.id = 0x201;
    change_ACramp.buf[0] = 0x35;
    change_ACramp.buf[1] = byte1;
    change_ACramp.buf[2] = byte2;

    can1.write(change_ACramp);
}
*/

void set_powerMODES(int pos){
    CAN_message_t Nlim_sent;
    CAN_message_t I_con_eff_sent;
    CAN_message_t I_max_peak_sent;

    uint8_t byte1_Nlim = (N_lim_values[pos] >> 8) & 0xFF;
    uint8_t byte2_Nlim = N_lim_values[pos] & 0xFF;

    uint8_t byte1_I_con_eff = (i_con_eff_values[pos] >> 8) & 0xFF;
    uint8_t byte2_I_con_eff = i_con_eff_values[pos] & 0xFF;

    uint8_t byte1_I_maxPK = (i_max_peak_values[pos] >> 8) & 0xFF;
    uint8_t byte2_I_maxPK = i_max_peak_values[pos] & 0xFF;

    Nlim_sent.id = 0x201;
    Nlim_sent.len = 3;
    Nlim_sent.buf[0] = 0x34;
    Nlim_sent.buf[1] = byte2_Nlim;
    Nlim_sent.buf[2] = byte1_Nlim;

    I_con_eff_sent.id = 0x201;
    I_con_eff_sent.len = 3;
    I_con_eff_sent.buf[0] = 0xC5;
    I_con_eff_sent.buf[1] = byte2_I_con_eff;
    I_con_eff_sent.buf[2] = byte1_I_con_eff;
    
    I_max_peak_sent.id = 0x201;
    I_max_peak_sent.len = 3;
    I_max_peak_sent.buf[0] = 0xC4;
    I_max_peak_sent.buf[1] = byte2_I_maxPK;
    I_max_peak_sent.buf[2] = byte1_I_maxPK;

    can1.write(I_max_peak_sent); 
    can1.write(I_con_eff_sent);
    can1.write(Nlim_sent);  

    Serial.print("mode set!"); 

}

int mapSensorValueToSwitchNumber(int sensorValue) {
    int rotswitchNumber = 0;

    float posicao = sensorValue;

    posicao /= 93;

    rotswitchNumber = round(posicao);
    return rotswitchNumber;
}

int mapSensorDashToSwitchNumber(int sensorValue) {
    int rotswtichNumber = 0;

    float posicao = sensorValue;

    posicao = (posicao - 225) / 48;

    rotswtichNumber = round(posicao);
    if(rotswtichNumber < 0) return 0;
    if(rotswtichNumber > 5) return rotswtichNumber-1;
    return rotswtichNumber;
}

void displaySetup() {
    myNex.begin(9600);
    pinMode(switchPin, INPUT);
    pinMode(dashPin, INPUT);

    int sensor1 = analogRead(switchPin);
    actual_mode_wheel = mapSensorValueToSwitchNumber(sensor1);
    
    int sensor2 = analogRead(dashPin);
    actual_mode_dash = mapSensorDashToSwitchNumber(sensor2);
}

void control_modes() {
    int initial_pos_dash = mapSensorDashToSwitchNumber(actual_mode_dash);
    int initial_pos_wheel = mapSensorValueToSwitchNumber(actual_mode_wheel);

    int wheel_Value = analogRead(switchPin);
    int dash_value = analogRead(dashPin);

    // Map the sensor value to the switch position
    int switchPosition_wheel = mapSensorValueToSwitchNumber(wheel_Value);
    int switchPosition_dash = mapSensorDashToSwitchNumber(dash_value);
    
    
    if(switchPosition_wheel != actual_mode_wheel){
        actual_mode_wheel = wheel_Value;
        int switchPosition_wheel = mapSensorValueToSwitchNumber(wheel_Value);

    } 
    
    if(switchPosition_dash != actual_mode_dash){
        actual_mode_dash = dash_value;
        int switchPosition_dash = mapSensorDashToSwitchNumber(dash_value);
        set_powerMODES(switchPosition_dash);

    } 
    
}


void displayUpdate() {
    myNex.NextionListen();

    int sensorValue = analogRead(switchPin);

    int switchPosition_wheel = mapSensorValueToSwitchNumber(sensorValue);

    char mode[28];

    sprintf(mode, "MODE %d", switchPosition_wheel);

    myNex.writeStr("t3.txt", mode);

    myNex.writeNum("n0.val", speedInt);
    myNex.writeNum("x0.val", soc * 10);
    myNex.writeNum("x1.val", highTemp * 10);
    myNex.writeNum("x2.val", packVoltage);
    myNex.writeNum("x3.val", current);
    myNex.writeNum("x4.val", motorTemp);
    myNex.writeNum("x5.val", powerStageTemp);
    myNex.writeNum("x7.val", ACCurrent);
    myNex.writeNum("x8.val", rpm);
    myNex.writeNum("x9.val", highTemp);
    myNex.writeNum("x10.val", lowTemp);
}

