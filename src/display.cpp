#include "display.h"
#include <EasyNextionLibrary.h>
#include <FlexCAN_T4.h>
#include "apps.h"

#define NEXTION_PORT Serial1

EasyNex myNex(NEXTION_PORT);

double speedInt = 0;
int tempInt = 0;
int socInt = 1002;
int current_page = 0;
int switchPin = 14;
int sensorValue = 0;
int current = 0;

extern CAN_message_t request_actual_speed;
extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

void setup_display() {
    myNex.begin(9600);
    // Serial.begin(38400);
    pinMode(switchPin, INPUT);
    myNex.writeNum("n0.val", speedInt);
    myNex.writeNum("x1.val", tempInt);
    myNex.writeNum("x0.val", socInt);
    myNex.writeNum("x2.val", current);
}

void control_display() {
    myNex.NextionListen();

    int sensorValue = analogRead(switchPin);

    // Map the sensor value to the switch position
    int switchPosition = mapSensorValueToSwitchNumber(sensorValue);

    char mode[28];

    sprintf(mode, "MODE %d", switchPosition);

    myNex.writeStr("t3.txt", mode);

    can1.write(request_actual_speed);

    speedInt = speedInt * 2000 / BAMOCAR_MAX;

    myNex.writeNum("n0.val", speedInt);
    myNex.writeNum("x1.val", tempInt);
    myNex.writeNum("x0.val", socInt);
    myNex.writeNum("x2.val", current);
}

int mapSensorValueToSwitchNumber(int sensorValue) {
    int rotswitchNumber = 0;

    float posicao = sensorValue;

    posicao /= 93;

    rotswitchNumber = round(posicao);
    return rotswitchNumber;
}
