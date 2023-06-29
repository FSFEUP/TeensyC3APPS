#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "apps.h"
#include "display.h"
#include "r2d.h"

r2d_mode curr_state;
extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

void setup() {
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);
    can_setup();
    // setup_display();
}

void loop() {
    //play_r2d_sound();
    /*
    int apps_value = read_apps();

    if (apps_value >= 0) {
        curr_state = r2d_state_machine(curr_state, apps_value);
    } else {
        Serial.println("ERROR: apps_implausibility");
    }
    delay(20);
    */
   CAN_message_t msg_received;
   if(can1.read(msg_received)) {
        Serial.printf("msg ID: %d",msg_received.id);
   }
    // else
    // send_to_bamocar(0);
    // String output = String(apps_value_1, DEC) + "\t" + String(apps_value_2, DEC);
    // Serial.println(output);
}
