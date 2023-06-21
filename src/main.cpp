#include <Arduino.h>

#define N_SAMPLES 5

int avgBuffer1[N_SAMPLES] = {0};
int avgBuffer2[N_SAMPLES] = {0};

int average(int* buffer, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += buffer[i];
    }
    return sum / n;
}

void buffer_insert(int* buffer, int n, int value) {
    for (int i = 0; i < n - 1; i++) {
        buffer[i] = buffer[i + 1];
    }
    buffer[n - 1] = value;
}

void setup() {
    Serial.begin(9600);
    pinMode(A17, INPUT);
    pinMode(A16, INPUT);
}

void loop() {
    int v_apps1 = 0;
    int v_apps2 = 0;

    v_apps1 = analogRead(A16);
    v_apps2 = analogRead(A17);

    buffer_insert(avgBuffer1, N_SAMPLES, v_apps1);
    buffer_insert(avgBuffer2, N_SAMPLES, v_apps2);

    v_apps1 = average(avgBuffer1, N_SAMPLES);
    v_apps2 = average(avgBuffer2, N_SAMPLES);

    String output = "s1: " + String(v_apps1, DEC) + "\ts2: " + String(v_apps2, DEC);
    Serial.println(output);

    delay(1);
}
