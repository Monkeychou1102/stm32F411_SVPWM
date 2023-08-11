#ifndef __PWM_FUNC_H
#define __PWM_FUNC_H

#include "../Inc/sys_init.h"

void PWM_Init(void);
void PWM_SetDutyPwm1(float input_percent);
void PWM_SetDutyPwm2(float input_percent);
void PWM_SetDutyPwm3(float input_percent);

#endif /* __PWM_FUNCTION */
