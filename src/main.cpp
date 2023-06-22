#include <Arduino.h>
#include <FlexCAN_T4.h>

#include "apps.h"
#include "r2d.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canbus;

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);
}

void sendBAMO(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    Serial.print("byte1: ");
    Serial.print(byte1, HEX);
    Serial.print("\n byte2: ");
    Serial.print(byte2, HEX);
    // definir a mensagem de acordo com o que o BAMOCAR pede
    // speed command value

    Serial.printf("\n Value sent: %d", value_bamo);

    CAN_message_t msg;
    msg.id = 0x201;
    msg.len = 3;
    msg.buf[0] = 0x31;
    msg.buf[1] = byte2;
    msg.buf[2] = byte1;

    Serial.print("Sent message with ID 0x");
    Serial.print(msg.id, HEX);
    Serial.print(": ");
    for (int i = 0; i < msg.len; i++) {
        Serial.print(msg.buf[i]);
    }

    canbus.write(msg);

    Serial.println("\n Message sent!");
    delay(10);
}

void loop() {
    int apps_value_1 = analogRead(A17);
    int apps_value_2 = analogRead(A16);

    // int apps_value = read_apps();
    // if (apps_value > 0)
    //     sendBAMO(apps_value);
    // else
    //     sendBAMO(0);

    String output = String(apps_value_1, DEC) + "\t" + String(apps_value_2, DEC);
    Serial.println(output);
}
