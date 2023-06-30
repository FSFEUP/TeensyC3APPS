#include <Arduino.h>
#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "display.h"
#include "r2d.h"

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define BAMOCAR_ATTENUATION_FACTOR 1
#define APPS_READ_DELAY_MS 10

APPS_Handler APPS(APPS_1_PIN, APPS_2_PIN, BAMOCAR_ATTENUATION_FACTOR, APPS_READ_DELAY_MS);
elapsedMillis CAN_Timer;
const int can_transmit_period = 20;

void setup() {
    delay(1000);

    pinMode(DASH_SELECTOR_PIN, INPUT);
    pinMode(PRECHARGE_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(R2D_PIN, INPUT);

    canbus_setup();

    APPS.init();

    Serial.begin(9600);
    Serial.println("Awake");

    // setup_display();
}

// -------------------------------------------------------------

void loop() {
    // play_r2d_sound();
    int apps_value = APPS.read();
    if (APPS.get_flag() == APPS_OK) {
        send_msg(apps_value);
        // curr_state = r2d_state_machine(curr_state, apps_value);
    } else if (APPS.get_flag() != APPS_WAITING_NEXT_READ) {
        send_msg(0);
        Serial.print("APPS error: ");
        Serial.println(APPS.get_flag());
    }
}
