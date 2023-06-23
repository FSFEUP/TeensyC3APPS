#include <EasyNextionLibrary.h>
#define NEXTION_PORT Serial1

EasyNex myNex(NEXTION_PORT);

int speedInt = 0;
int tempInt = 0;
int socInt = 1002;
int current_page = 0;
int switchPin = 14;
int sensorValue = 0;

void setup_display() {
    myNex.begin(9600);
    Serial.begin(38400);
    pinMode(switchPin, INPUT);
    myNex.writeNum("n0.val", speedInt);
    myNex.writeNum("x1.val", tempInt);
    myNex.writeNum("x0.val", socInt);
}

void control_display() {
    myNex.NextionListen();

    int sensorValue = analogRead(switchPin);

    // Map the sensor value to the switch position
    int switchPosition = mapSensorValueToSwitchNumber(sensorValue);

    char mode[28];

    sprintf(mode, "MODE %d", switchPosition);

    myNex.writeStr("t3.txt", mode);

    // Increment speed, temperature, and SOC on page 0
    speedInt++;
    if (speedInt == 201) {
      speedInt = 0;
    }

    tempInt++;
    if (tempInt == 1002) {
      tempInt = 0;
    }

    socInt--;
    if (socInt == 0) {
      socInt = 1002;
    }

    if(speedInt > 40 && speedInt < 90){
      myNex.writeNum("t0.pco", 63488);
      myNex.writeStr("t0.txt", "NOK");
    } else{
      myNex.writeNum("t0.pco", 1032);
      myNex.writeStr("t0.txt", "OK");
    }

    myNex.writeNum("n0.val", speedInt);
    myNex.writeNum("x1.val", tempInt);
    myNex.writeNum("x0.val", socInt);
}

int mapSensorValueToSwitchNumber(int sensorValue) {
  int rotswitchNumber = 0;

  float posicao = sensorValue;

  posicao /= 93;

  rotswitchNumber  = round(posicao);
  return rotswitchNumber;
}