#include "main.h"

#define STORED_CHECKSUM 455

extern void Application_Start(void);

uint32_t Calculate_Checksum(void)
{
    uint32_t sum = 0;

    for(int i = 0; i < 10; i++)
    {
        sum += i;
    }

    return sum;
}

void Bootloader_Run(void)
{
    uint32_t checksum;

    checksum = Calculate_Checksum();

    if(checksum == STORED_CHECKSUM)
    {
        // show bootloader success
        for(int i=0;i<2;i++)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
            HAL_Delay(500);
        }

        Application_Start();
    }
    else
    {
        // firmware invalid
        while(1)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
            HAL_Delay(200);
        }
    }
}
