#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "display.h"
#include "r2d.h"

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define BAMOCAR_ATTENUATION_FACTOR 1
#define APPS_READ_PERIOD_MS 20

elapsedMillis APPS_TIMER;

void setup() {
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);
    // can_setup();
    // setup_display();
}

void loop() {
    // play_r2d_sound();

    if (APPS_TIMER > APPS_READ_PERIOD_MS) {
        APPS_TIMER = 0;
        int apps_value = read_apps();

        if (apps_value >= 0) {
            send_msg(apps_value);
        } else {
            Serial.println("ERROR: apps_implausibility");
        }
    }
}