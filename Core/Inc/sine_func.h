#ifndef __XSINE_FUNC_H
#define __XSINE_FUNC_H

#include "../Inc/main.h"

#define SINE_TAB_SIZE (1024)

#define SINE_90_CNT 90
#define SINE_180_CNT 180
#define SINE_270_CNT 270
#define SINE_360_CNT 360

#define SINE_GAIN ((SINE_TAB_SIZE - 1) / 90.0) // 0 ~ 1023 is 0 ~ 90 degree

// Degree -> 0 ~ 90 degree
// Value  -> 0 ~ 1

float SINE_Process(float degree);

#endif /* __SINE_FUNC_H */
