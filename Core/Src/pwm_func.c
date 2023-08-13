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
    float Ts;

    float Angle; // degrees
    uint8_t Sector;

    float Tga;
    float Tgb;
    float Tgc;

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

void initSvpwm(void)
{
    svpwm.Ts = MAX_PWM_DUTY; // PWM period
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

    initSvpwm();
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
    svpwm.Angle = angle;
    svpwm.Sector = (angle / SINE_60_DEG) + 1; // sector: 1 ~ 6
    svpwm.T1 = vref * SINE_GetSineValue(alpha);
    svpwm.T2 = vref * SINE_GetSineValue(angle) / SINE_GetSine60Value();
    svpwm.Tz = MAX_PWM_DUTY - svpwm.T1 - svpwm.T2; // T0 = Tz
    svpwm.Ts = MAX_PWM_DUTY;                       // PWM period
}

typedef enum
{
    SVPWM_SECTOR_1 = 1,
    SVPWM_SECTOR_2 = 2,
    SVPWM_SECTOR_3 = 3,
    SVPWM_SECTOR_4 = 4,
    SVPWM_SECTOR_5 = 5,
    SVPWM_SECTOR_6 = 6
} svpwm_sector_t;

void UpdSvpwmDuty(void)
{
    float t1 = svpwm.T1;
    float t2 = svpwm.T2;
    float tz = svpwm.Tz / 2;
    float ts = svpwm.Ts;

    switch (svpwm.Sector)
    {
    case SVPWM_SECTOR_1:
        svpwm.Tga = tz;
        svpwm.Tgb = tz + t1;
        svpwm.Tgc = ts - tz;
        break;

    case SVPWM_SECTOR_2:
        svpwm.Tga = tz + t2;
        svpwm.Tgb = tz;
        svpwm.Tgc = ts - tz;
        break;

    case SVPWM_SECTOR_3:
        svpwm.Tga = ts - tz;
        svpwm.Tgb = tz;
        svpwm.Tgc = tz + t1;
        break;

    case SVPWM_SECTOR_4:
        svpwm.Tga = ts - tz;
        svpwm.Tgb = ts + t2;
        svpwm.Tgc = tz;
        break;

    case SVPWM_SECTOR_5:
        svpwm.Tga = tz + t1;
        svpwm.Tgb = ts - tz;
        svpwm.Tgc = tz;
        break;

    case SVPWM_SECTOR_6:
        svpwm.Tga = tz;
        svpwm.Tgb = ts - tz;
        svpwm.Tgc = tz + t2;
        break;

    default:
        svpwm.Tga = 0;
        svpwm.Tgb = 0;
        svpwm.Tgc = 0;
        break;
    }

    // Update PWM duty based on the SVPWM switch rule
    PWM_SetDutyPwm1(svpwm.Tga); // Phase A
    PWM_SetDutyPwm2(svpwm.Tgb); // Phase B
    PWM_SetDutyPwm3(svpwm.Tgc); // Phase C
}

/*
MCU: stm32f411RE (@100MHz)
Test this process in 50KHz Timer ISR
It takes about 5.6us to compute the SVPWM switching rules.
*/
void PWM_SvpwmProcess(float vref, float angle)
{
    CalculateT1T2Tz(vref, angle);
    UpdSvpwmDuty();
}
