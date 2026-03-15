#include "drivers/gpio/gpio.h"
#include "drivers/interrupt/external_interrupt.h"
#include "bsp/nano.h"
#include "drivers/usart/usart.h"
#include "drivers/timer/timer0.h"
#include <avr/io.h>
#include "drivers/adc/adc.h"
#include "stdio.h"
int main()
{
    Timer0_Init();
    USART_Init_Default();
    ADC_Init();
    uint16_t adc_read;
    char message[30] = { 0 };
    while (1) {
        adc_read = ADC_Read(0);
        sprintf(message, "%d \n", adc_read);
        USART_Transmit(&message, sizeof(message));
        DelayT0(300);
    }
}