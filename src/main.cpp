#include <Arduino.h>

#define N_SAMPLES 10

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);
}

float desvio(int v1, int v2) {
    int v_max = max(v1, v2);
    int v_min = min(v1, v2) + 200;

    return ((v_max - v_min) / (float)v_min) * 100;
}

void loop() {
    int v_apps1 = 0;
    int v_apps2 = 0;

    for (int i = 0; i < N_SAMPLES; i++) {
        v_apps1 += analogRead(A17);
        v_apps2 += analogRead(A16);
        delay(1);
    }

    v_apps1 /= N_SAMPLES;
    v_apps2 /= N_SAMPLES;

    desvio(v_apps1, v_apps2);

    String output = String(v_apps1, DEC) + "\t" + String(v_apps2, DEC) + "\t" + desvio(v_apps1, v_apps2) + "%";
    Serial.println(output);
}
