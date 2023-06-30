#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#include "apps.h"
#include "display.h"
#include "r2d.h"
#include "can.h"

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define BAMOCAR_ATTENUATION_FACTOR 1
#define APPS_READ_DELAY_MS 10

elapsedMillis CAN_Timer;

void setup() {
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);
    // can_setup();
    // setup_display();
}

void loop() {
    // play_r2d_sound();

    int apps_value = read_apps();

    if (apps_value >= 0) {
        // curr_state = r2d_state_machine(curr_state, apps_value);
    } else {
        Serial.println("ERROR: apps_implausibility");
    }
    delay(20);
}
