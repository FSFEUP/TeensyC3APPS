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

int transform_apps_2(int apps2) {
    return apps2 + APPS_LINEAR_OFFSET;
}

bool plausibility(int v_apps1, int v_apps2) {
    if (v_apps1 < v_apps2)
        return false;

    if (v_apps1 > APPS_1_UPPER_BOUND || v_apps1 < APPS_1_LOWER_BOUND)
        return false;

    if (v_apps2 > APPS_2_UPPER_BOUND || v_apps2 < APPS_2_LOWER_BOUND)
        return false;

    if (v_apps1 >= APPS1_DEAD_THRESHOLD)
        return v_apps2 >= APPS1_DEADZONE_EQUIVALENT - APPS_MAX_ERROR_ABS && v_apps2 <= APPS_2_UPPER_BOUND;

    if (v_apps2 <= APPS2_DEAD_THRESHOLD)
        return v_apps1 >= APPS_1_LOWER_BOUND && v_apps1 <= APPS2_DEADZONE_EQUIVALENT + APPS_MAX_ERROR_ABS;

    int v2_expected = transform_apps_2(v_apps2);
    int plausibility_value = abs(v2_expected - v_apps1) * 100 / v_apps1;

    return (plausibility_value < APPS_MAX_ERROR_PERCENT);
}

int read_apps() {
    int v_apps1 = analogRead(APPS_1_PIN);
    int v_apps2 = analogRead(APPS_2_PIN);

    if (!plausibility(v_apps1, v_apps2))
        return false;

    buffer_insert(avgBuffer1, AVG_SAMPLES, v_apps1);
    buffer_insert(avgBuffer2, AVG_SAMPLES, v_apps2);

    v_apps1 = average(avgBuffer1, AVG_SAMPLES);
    v_apps2 = average(avgBuffer2, AVG_SAMPLES);

    if (!plausibility(v_apps1, v_apps2))
        return -1;

    return (BAMOCAR_MAX / 5) - (v_apps1 * (BAMOCAR_MAX / 5) / APPS_1_UPPER_BOUND);
}
