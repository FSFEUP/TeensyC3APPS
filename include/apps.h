#ifndef _APPS_H_
#define _APPS_H_

#define AVG_SAMPLES 5

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define APPS_1_UPPER_BOUND 1023
#define APPS_1_LOWER_BOUND 35

#define APPS_2_UPPER_BOUND 850
#define APPS_2_LOWER_BOUND 10

#define BAMOCAR_MAX 32760

#define REG_M1 0.00124
#define REG_M2 0.00141

#define REG_B1 -0.0619
#define REG_B2 -0.4678

int read_apps();
int average(int* buffer, int n);
void buffer_insert(int* buffer, int n, int value);

#endif  // _APPS_H_