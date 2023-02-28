#include "main.h"

#define MAX_PWM_DUTY 1000

float checkPwmDuty(float input_percent)
{
// Input 0 ~ 100%, then conver to 0 ~ 1 and return the result.
  
    float val = 0; 

    if (input_percent > 100)
    {
        input_percent = 100;
    }

    val = input_percent / 100;

    return val;
}

void PWM_setDutyPwm1(float input_percent)
{
    static float previous_duty = 0;
    float val = 0;

    if (input_percent == previous_duty)
    {
        return;
    }

    val = checkPwmDuty(input_percent);
    previous_duty = val;
    htim1.Instance->CCR1 = (uint32_t)(val * MAX_PWM_DUTY);
}

void PWM_setDutyPwm2(float input_percent)
{
    static float previous_duty = 0;
    float val = 0;

    if (input_percent == previous_duty)
    {
        return;
    }

    val = checkPwmDuty(input_percent);
    previous_duty = val;
    htim1.Instance->CCR2 = (uint32_t)(val * MAX_PWM_DUTY);
}

void PWM_setDutyPwm3(float input_percent)
{
    static float previous_duty = 0;
    float val = 0;

    if (input_percent == previous_duty)
    {
        return;
    }

    val = checkPwmDuty(input_percent);
    previous_duty = val;
    htim1.Instance->CCR3 = (uint32_t)(val * MAX_PWM_DUTY);
}
