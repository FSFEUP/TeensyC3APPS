#ifndef _APPS_H_
#define _APPS_H_

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define APPS_1_UPPER_BOUND 800
#define APPS_1_LOWER_BOUND 240

#define APPS_2_UPPER_BOUND 785
#define APPS_2_LOWER_BOUND 220

#define APPS1_DEAD_THRESHOLD 788
#define APPS2_DEAD_THRESHOLD 240

#define APPS1_DEADZONE_EQUIVALENT 660
#define APPS2_DEADZONE_EQUIVALENT 360

#define APPS_LINEAR_OFFSET 124

#define APPS_MAX 770
#define APPS_MIN 270
#define BAMOCAR_MAX 32760
#define BAMOCAR_MIN 0

#define APPS_MAX_ERROR_PERCENT (10 - 1.173)
#define APPS_MAX_ERROR_ABS (APPS_1_UPPER_BOUND * APPS_MAX_ERROR_PERCENT / 100)

#define AVG_SAMPLES 5

#define APPS_IMPLAUSIBLE_TIMEOUT_MS 100

#define APPS_BRAKE_PLAUSIBILITY_TIMEOUT_MS 500

int readApps();

#endif  // _APPS_H_