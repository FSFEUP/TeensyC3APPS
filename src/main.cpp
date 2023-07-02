#include <Arduino.h>
#include <FlexCAN_T4.h>

#include <Bounce2.h>

#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "display.h"

// #include "r2d.h"

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define R2D_PIN 32

#define BAMOCAR_ATTENUATION_FACTOR 1
#define APPS_READ_PERIOD_MS 20

bool BTB_READY = false;
bool TRANSMISSION_ENABLED = false;

uint8_t buzzerPin = 4;

elapsedMillis APPS_TIMER;

Bounce r2d_button = Bounce();

extern CAN_message_t request_bamo;
extern CAN_message_t bamo_apps;
extern CAN_message_t msg_transmissionRequest_BTB;
extern CAN_message_t receiving_BTB;
extern CAN_message_t transmitting_disable;
extern CAN_message_t transmitting_request_enable;
extern CAN_message_t receiving_enable;
extern CAN_message_t transmitting_enable;
extern CAN_message_t transmitting_ACC_ramp;
extern CAN_message_t transmitting_DEC_ramp;

enum status {
    IDLE,
    DRIVING
};

status r2d_status;
elapsedMillis r2d_timer;
const int R2D_TIMEOUT = 500;

void play_r2d_sound() {
    digitalWrite(buzzerPin, HIGH);  // Turn off the buzzer for the other half of the period
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
}

void setup() {
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);

    pinMode(buzzerPin, OUTPUT);
    canbus_setup();

    r2d_button.attach(R2D_PIN, INPUT);
    r2d_button.interval(10);

    r2d_status = IDLE;
    init_can_messages();
    // setup_display();
}

void loop() {
    switch (r2d_status) {
        case IDLE:
            r2d_button.update();
            if (r2d_button.fell()) {
                if (r2d_timer < R2D_TIMEOUT) {
                    play_r2d_sound();
                    prepare_BAMO();
                    r2d_status = DRIVING;
                } else {
                    Serial.println("ERROR: r2d not available");
                }
            }
            /* code */
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
            // send_msg(0);
            // can1.write(transmitting_disable);
            break;

        default:
            break;
    }
}