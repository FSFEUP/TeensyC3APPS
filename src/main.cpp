#include <Arduino.h>

#include "apps.h"
#include "r2d.h"
#include "display.h"

r2d_mode curr_state;

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);
    can_setup();
    setup_display();
}

void loop() {
    // int apps_value_1 = analogRead(A17);
    // int apps_value_2 = analogRead(A16);
    int apps_value = read_apps();
    if (apps_value > 0) {
        curr_state = r2d_state_machine(curr_state, apps_value);
    }
    else {
        Serial.print("ERROR: apps_value < 0");
        delay(1);
    }
    delay(1);
    // else
    // send_to_bamocar(0);
    // String output = String(apps_value_1, DEC) + "\t" + String(apps_value_2, DEC);
    // Serial.println(output);
}
