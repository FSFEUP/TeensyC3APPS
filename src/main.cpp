#include <Arduino.h>

#include "apps.h"

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);
}

void loop() {
    int apps_value = apps();

    String output = "s1: " + String(apps_value, DEC) + "\ts2: " + String(apps_value, DEC);
    Serial.println(output);

    delay(1);
}
