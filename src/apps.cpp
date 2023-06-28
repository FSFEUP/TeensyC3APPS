#include "apps.h"
#include <Arduino.h>

int avgBuffer1[AVG_SAMPLES] = {0};
int avgBuffer2[AVG_SAMPLES] = {0};

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

// usar valor da em y para calcular x
double apps1_x_val(int apps1_y) {
    // y == mx + b
    // x == (y - b) / m
    return (apps1_y - REG_B1) / REG_M1;
}

// usar valor da em y para calcular x
double apps2_x_val(int apps2_y) {
    return (apps2_y - REG_B2) / REG_M2;
}

double apps_deviation(double apps1_x, double apps2_x) {
    return abs(apps1_x - apps2_x) / ((apps1_x + apps2_x) / 2) * 100;
}

bool plausibility(int v_apps1, int v_apps2) {
    // plausbility check
    // if (v_apps1 < v_apps2)
    //     return false;

    // if (v_apps1 >= 1022 || v_apps2 >= 850)
    //     return false;

    // if (v_apps1 <= 35 || v_apps2 <= 10)
    //     return false;

    double plausibility_value = apps_deviation(apps1_x_val(v_apps1), apps2_x_val(v_apps2));

    Serial.print("Apps1 (xy): ");
    // Serial.print(apps1_x_val(v_apps1));
    // Serial.print(" ");
    Serial.print(v_apps1);
    Serial.print("\tApps2 (xy): ");
    // Serial.print(apps2_x_val(v_apps2));
    // Serial.print(" ");
    Serial.println(v_apps2);
    // Serial.print("\tSensor deviation: ");
    // Serial.print(plausibility_value);
    // Serial.println("%");

    return (plausibility_value < 10);
}

int read_apps() {
    int v_apps1 = analogRead(A16);
    int v_apps2 = analogRead(A17);

    if (!plausibility(v_apps1, v_apps2)) return false;

    buffer_insert(avgBuffer1, AVG_SAMPLES, v_apps1);
    buffer_insert(avgBuffer2, AVG_SAMPLES, v_apps2);

    v_apps1 = average(avgBuffer1, AVG_SAMPLES);
    v_apps2 = average(avgBuffer2, AVG_SAMPLES);

    if (!plausibility(v_apps1, v_apps2)) return -1;

    return (BAMOCAR_MAX / 5) - (v_apps1 * (BAMOCAR_MAX / 5) / APPS_1_UPPER_BOUND);
}
