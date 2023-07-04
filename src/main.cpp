#include <Arduino.h>
#include <FlexCAN_T4.h>

#include <Bounce2.h>

#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "display.h"

#define buzzerPin 4

#define R2D_PIN 32
#define R2D_TIMEOUT 500

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define STARTUP_DELAY_MS 10000

#define BAMOCAR_ATTENUATION_FACTOR 1
#define APPS_READ_PERIOD_MS 20

volatile bool BTB_ready = false;
volatile bool transmission_enabled = false;
volatile bool disabled = false;
volatile bool r2d_override = false;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t status_request;
extern CAN_message_t status_report;

extern CAN_message_t torque_request;

extern CAN_message_t BTB_status;
extern CAN_message_t BTB_response;

extern CAN_message_t disable;
extern CAN_message_t no_disable;

extern CAN_message_t transmission_request_enable;
extern CAN_message_t enable_response;

enum status {
    IDLE,    // waiting for r2d && ATS off
    DRIVING  // r2d pressed && ATS on
};

status r2d_status;
elapsedMillis r2d_timer;

elapsedMillis APPS_TIMER;
Bounce r2d_button = Bounce();

void play_r2d_sound();

void setup() {
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);

    pinMode(buzzerPin, OUTPUT);
    canbus_setup();

    r2d_button.attach(R2D_PIN, INPUT);
    r2d_button.interval(1);

    r2d_status = IDLE;

    init_can_messages();

    delay(STARTUP_DELAY_MS);

    can1.write(disable);
    can1.write(status_request);

    // setup_display();
}

void loop() {
    switch (r2d_status) {
        case IDLE:
            r2d_button.update();
            if (r2d_override)
                r2d_status = DRIVING;
            if (r2d_button.fell()) {
                if (r2d_timer < R2D_TIMEOUT) {
                    play_r2d_sound();
                    BAMO_init_operation();
                    r2d_status = DRIVING;
                } else {
                    Serial.println("ERROR: r2d not available");
                }
            }
            break;

        case DRIVING:
            if (APPS_TIMER > APPS_READ_PERIOD_MS) {
                APPS_TIMER = 0;
                int apps_value = read_apps();

                if (apps_value >= 0) {
                    send_msg(apps_value);
                } else {
                    send_msg(0);
                    Serial.println("ERROR: apps_implausibility");
                }
            }
            break;

        default:
            break;
    }
}

void play_r2d_sound() {
    digitalWrite(buzzerPin, HIGH);  // Turn off the buzzer for the other half of the period
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
}