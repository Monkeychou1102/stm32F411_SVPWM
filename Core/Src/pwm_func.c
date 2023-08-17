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
    float Tzero_vector;
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
    svpwm.Ts = MAX_PWM_DUTY; // 1/2 ofPWM period
}

void PWM_Init(void)
{
    startTimer1();

    startPWM1();
    startPWM2();
    startPWM3();

    /* Example Code For APIs */
    // PWM Futy: 0 ~ 1000
    // PWM_SetDutyPwm1(200); // For Test
    // PWM_SetDutyPwm2(500); // For Test
    // PWM_SetDutyPwm3(900); // For Test
    /* Example Code For APIs */

    initSvpwm();
}

void PWM_SetDutyPwm1(float duty)
{
    if (duty > MAX_PWM_DUTY)
    {
        duty = MAX_PWM_DUTY;
    }

    if (duty < 0)
    {
        duty = 0;
    }

    htim1.Instance->CCR1 = (uint32_t)(duty);
}

void PWM_SetDutyPwm2(float duty)
{
    if (duty > MAX_PWM_DUTY)
    {
        duty = MAX_PWM_DUTY;
    }

    if (duty < 0)
    {
        duty = 0;
    }

    htim1.Instance->CCR2 = (uint32_t)(duty);
}

void PWM_SetDutyPwm3(float duty)
{
    if (duty > MAX_PWM_DUTY)
    {
        duty = MAX_PWM_DUTY;
    }

    if (duty < 0)
    {
        duty = 0;
    }

    htim1.Instance->CCR3 = (uint32_t)(duty);
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
    svpwm.T2 = vref * SINE_GetSineValue(angle);              // / SINE_GetSine60Value();
    svpwm.Tzero_vector = MAX_PWM_DUTY - svpwm.T1 - svpwm.T2; // T0 = Tz
    svpwm.Ts = MAX_PWM_DUTY;                                 // PWM period
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
    float t0_half = svpwm.Tzero_vector / 2;
    float ts = svpwm.Ts;

    switch (svpwm.Sector)
    {
    case SVPWM_SECTOR_1:
        svpwm.Tga = t1 + t2 - t0_half;
        svpwm.Tgb = t2 + t0_half;
        svpwm.Tgc = t0_half;
        break;

    case SVPWM_SECTOR_2:
        svpwm.Tga = t1 + t0_half;
        svpwm.Tgb = t1 + t2 + t0_half;
        svpwm.Tgc = t0_half;
        break;

    case SVPWM_SECTOR_3:
        svpwm.Tga = t0_half;
        svpwm.Tgb = t1 + t2 + t0_half;
        svpwm.Tgc = t2 + t0_half;
        break;

    case SVPWM_SECTOR_4:
        svpwm.Tga = t0_half;
        svpwm.Tgb = t1 + t0_half;
        svpwm.Tgc = t1 + t2 + t0_half;
        break;

    case SVPWM_SECTOR_5:
        svpwm.Tga = t2 + t0_half;
        svpwm.Tgb = t0_half;
        svpwm.Tgc = t1 + t2 + t0_half;
        break;

    case SVPWM_SECTOR_6:
        svpwm.Tga = t1 + t2 + t0_half;
        svpwm.Tgb = t0_half;
        svpwm.Tgc = t1 + t0_half;
        break;

    default:
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
