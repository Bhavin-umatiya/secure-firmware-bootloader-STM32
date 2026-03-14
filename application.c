#include "main.h"

#include "main.h"

void Application_Start(void)
{
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
        HAL_Delay(1000);
    }
}
uint8_t DHT11_Read(void)
{
    uint8_t data = 0;

    for(int i=0;i<8;i++)
    {
        while(!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)));
        HAL_Delay(1);

        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))
            data |= (1 << (7-i));

        while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1));
    }

    return data;
}
