#include <Arduino.h>

#include "apps.h"
#include "r2d.h"

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);

    startup();
}

void loop() {
    // int apps_value = read_apps();

    // String output = "s1: " + String(apps_value, DEC) + "\ts2: " + String(apps_value, DEC);
    // Serial.println(output);

    

}
