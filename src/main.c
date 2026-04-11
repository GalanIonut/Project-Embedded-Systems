#include "bsp/nano.h"
#include "drivers/timer/timer0.h"
#include "drivers/servo/servo.h"
#include "utils/delay.h"
#include "drivers/gpio/gpio.h"
#include "drivers/adc/adc.h"
int main(void) {
    Timer0_Init();   /* system tick for Delay() */
    Servo_Init();    /* D9 / OC1A – 50 Hz, starts at 90° */
	ADC_Init();
	uint32_t value = 0;
	GPIO_Init(D4, GPIO_OUTPUT);
    while (1) {
	/*	value = ADC_Read(0);
		Servo_SetAngle((uint16_t)(value*180.0/1023.0));*/
		GPIO_Toggle(D4);
		Delay(1000);
		
    }

    return 0;
}
