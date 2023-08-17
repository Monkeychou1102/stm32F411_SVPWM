#ifndef __SINE_FUNC_H
#define __SINE_FUNC_H

#include "../Inc/main.h"

#define SINE_TAB_SIZE (1024)

#define SINE_60_DEG 60
#define SINE_90_DEG 90
#define SINE_180_DEG 180
#define SINE_270_DEG 270
#define SINE_360_DEG 360

#define SINE_GAIN ((SINE_TAB_SIZE - 1) / 90.0) // 0 ~ 1023 is mapped to 0 ~ 90 degree

// Degree -> 0 ~ 90 degree
// Value  -> 0 ~ 1

float SINE_GetSineValue(float degree);
float SINE_GetCosineValue(float degree);
float SINE_GetSine60Value(void);

#endif /* __SINE_FUNC_H */
