#include <Arduino.h>

#include "apps.h"
#include "r2d.h"

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);

    // startup();
}

void loop() {
    // int apps_value = read_apps();
    int apps_value_1 = analogRead(A17);
    int apps_value_2 = analogRead(A16);

    

    String output = String(apps_value_1, DEC) + "\t" + String(apps_value_2, DEC);
    Serial.println(output);
}
