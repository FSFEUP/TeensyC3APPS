#include <Arduino.h>
#include <FlexCAN_T4.h>

#include <Bounce2.h>

#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "debug.h"
#include "display.h"

#define buzzerPin 4

#define R2D_PIN 32
#define R2D_TIMEOUT 500

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define STARTUP_DELAY_MS 10000

#define APPS_READ_PERIOD_MS 20
#define BAMOCAR_ATTENUATION_FACTOR 1

volatile bool disabled = false;
volatile bool BTB_ready = false;
volatile bool transmission_enabled = false;

volatile bool r2d = false;
volatile bool r2d_override = false;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t status_request;

extern CAN_message_t disable;

extern CAN_message_t dc_bus_voltage_request;
extern CAN_message_t request_actual_speed;

enum status {
    IDLE,    // waiting for r2d && ATS off
    DRIVING  // r2d pressed && ATS on
};

status r2d_status;
elapsedMillis r2d_timer;

elapsedMillis APPS_TIMER;
Bounce r2d_button = Bounce();

void play_r2d_sound() {
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
}

void setup() {
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);

    pinMode(buzzerPin, OUTPUT);
    canbus_setup();

    r2d_button.attach(R2D_PIN, INPUT);
    r2d_button.interval(0.1);

    r2d_status = IDLE;

    delay(STARTUP_DELAY_MS);

    can1.write(disable);
    can1.write(status_request);
    can1.write(request_actual_speed);
    can1.write(dc_bus_voltage_request);

    displaySetup();
}

void loop() {
    displayControl();
    switch (r2d_status) {
        case IDLE:
            r2d_button.update();
            if ((r2d_button.fell() and r2d) or r2d_override) {
                play_r2d_sound();
                initBamocarD3();
                r2d_status = DRIVING;
                break;
            }
            break;

        case DRIVING:
            if (not r2d and not r2d_override) {
                r2d_status = IDLE;
                can1.write(disable);
                break;
            }

            if (APPS_TIMER > APPS_READ_PERIOD_MS) {
                APPS_TIMER = 0;
                int apps_value = readApps();

                if (apps_value >= 0)
                    send_msg(apps_value);
                else
                    send_msg(0);
                break;
            }
            break;

        default:
            ERROR("Invalid r2d_status");
            break;
    }
}
