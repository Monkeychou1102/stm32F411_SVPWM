#include "main.h"

void toggle_DebugPin1(void)
{
    static char cnt = 0;

    cnt++;
    if (cnt % 2)
    {
        HAL_GPIO_WritePin(GPIOA, DEBUG1_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, DEBUG1_Pin, GPIO_PIN_RESET);
    }
}
