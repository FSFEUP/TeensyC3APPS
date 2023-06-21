#ifndef _APPS_H_
#define _APPS_H_

#define AVG_SAMPLES 5

#define APPS_1_UPPER_BOUND 1010
#define APPS_1_LOWER_BOUND 35

#define APPS_2_UPPER_BOUND 850
#define APPS_2_LOWER_BOUND 10

#define BAMOCAR_MAX 32000

int apps();

int average(int* buffer, int n);
void buffer_insert(int* buffer, int n, int value);

#endif  // _APPS_H_