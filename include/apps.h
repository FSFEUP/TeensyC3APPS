#ifndef _APPS_H_
#define _APPS_H_

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define APPS_1_UPPER_BOUND 1023
#define APPS_1_LOWER_BOUND 38

#define APPS_2_UPPER_BOUND 971
#define APPS_2_LOWER_BOUND 0

#define APPS1_DEAD_THRESHOLD 1022
#define APPS2_DEAD_THRESHOLD 1

#define APPS1_DEADZONE_EQUIVALENT 781
#define APPS2_DEADZONE_EQUIVALENT 252

#define APPS_LINEAR_OFFSET 252

#define APPS_MAX_ERROR_PERCENT 4.5
#define APPS_MAX_ERROR_ABS (APPS_1_UPPER_BOUND * APPS_MAX_ERROR_PERCENT / 100)

#define AVG_SAMPLES 5
#define BAMOCAR_MAX 32760

int read_apps();
int average(int* buffer, int n);
void buffer_insert(int* buffer, int n, int value);

#endif  // _APPS_H_