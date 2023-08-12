#include "../Inc/sys_init.h"

#define MAX_PWM_DUTY 1000

/*
Timer counter mode is centeraling
        /\    /\
       /  \  /  \
      /    \/    \
The pwm frequency is 25KHz.
Value 0 ~ 1000 is mapping to duty 0 ~ 100%

The pwm frequency is 1/2 timer 1 ISR
*/

typedef struct svpwm_group_s
{
    float T1;
    float T2;
    float Tz;
    float angle; // degrees
    uint8_t sector;
} svpwm_group_t;

svpwm_group_t svpwm;

void startTimer1(void)
{
    HAL_TIM_Base_Start_IT(&htim1);
}

void startPWM1(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);    // CH1
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); // CH1N (Complementary)
}

void startPWM2(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);    // CH2
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // CH2N (Complementary)
}

void startPWM3(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);    // CH3
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); // CH3N (Complementary)
}

void PWM_Init(void)
{
    startTimer1();

    startPWM1();
    startPWM2();
    startPWM3();

    /* Example Code For APIs */
    // PWM
    // PWM_SetDutyPwm1(20); // For Test
    // PWM_SetDutyPwm2(50); // For Test
    // PWM_SetDutyPwm3(90); // For Test
    /* Example Code For APIs */
}

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

void PWM_SetDutyPwm1(float input_percent)
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

void PWM_SetDutyPwm2(float input_percent)
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

void PWM_SetDutyPwm3(float input_percent)
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

void CalculateT1T2Tz(float vref, float angle)
{
    float alpha = 60 - angle;

    if (alpha < 0)
    {
        alpha += 360;
    }

    // Update pwm related information
    svpwm.angle = angle;
    svpwm.sector = angle / SINE_60_DEG;
    svpwm.T1 = vref * SINE_GetSineValue(alpha);
    svpwm.T2 = vref * SINE_GetSineValue(angle) / SINE_GetSine60Value();
    svpwm.Tz = MAX_PWM_DUTY - svpwm.T1 - svpwm.T2;
}
