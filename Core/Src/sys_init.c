#include "../Inc/main.h"

/*
Information of timer_1 interrupt

Update_Event = TIM_CLK / ((PSC + 1)*(ARR + 1)*(PCR + 1))

Where
TIM_CLK = timer clock input (Hz)
Update_Event = target isnterrupt frequency (Hz)
PSC = 16-bit prescaler register
ARR = 16/32-bit Autoreload register
PCR = 16-bit repetition counter

If
TIM_CLK = 100 MH
Update_Event = 50 KHz
PCR = 0
PSC = 1

Then
(ARR + 1) = TIM_CLK / ((PSC + 1)*(Update_Event)*(PCR + 1))
          = 100 MHz / ((50 KHz)*2)
          = 1000
*/

void startTimer1(void)
{
    HAL_TIM_Base_Start_IT(&htim1);
}

void startPWM1(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void startPWM2(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void startPWM3(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void sys_Init(void)
{
    startTimer1();

    startPWM1();
    startPWM2();
    startPWM3();
}
