#include <utils/delay.h>
#include <drivers/gpio/gpio.h>

void setup(gpio_port_t porttrig, uint8_t pintrig,
		   gpio_port_t portecho, uint8_t pinecho) {
	GPIO_Init(porttrig, pintrig, GPIO_OUTPUT);
	GPIO_Init(portecho, pinecho, GPIO_INPUT);
	Delay(6000);
}

uint8_t getdistance(gpio_port_t porttrig, uint8_t pintrig,
					gpio_port_t portecho, uint8_t pinecho) {
	GPIO_Write(porttrig, pintrig, GPIO_HIGH);
	Delay10us();
	GPIO_Write(porttrig, pintrig, GPIO_LOW);
	uint16_t count = 0;
	uint8_t distanceincm = 0;
	// 21 clock cycles per count increment => 0.0625 * 21 us = 1.3125 us per count.
	while(GPIO_Read(portecho,pinecho) == GPIO_HIGH)
	{
		count++;
	}
	// we must convert from clock cycles to us. if 1 = 1.3125 us 
	//                                             x = 1 us
	// x = 1us/1.3125us = 0.7619047619
	count = (uint16_t)(count * 0.7619047619);
	distanceincm = (uint8_t)(count / 58);
	return distanceincm;
}