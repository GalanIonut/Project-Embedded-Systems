#include "hcsr04.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timer/timer1.h"
#include "drivers/timer/timer0.h"
#include "utils/delay.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * HC-SR04 ULTRASONIC SENSOR DRIVER
 *
 * ECHO → D8 (PB0 / ICP1)  [FIXED — hardware pin]
 * TRIG → orice pin GPIO   [parametru]
 *
 * Foloseste Timer1 ICP prin timer1.h (fara acces direct la registre).
 */

static volatile uint8_t  capture_state = 0;  /* 0=idle, 1=rising capturat, 2=done */
static volatile uint16_t time_rising   = 0;
static volatile uint16_t time_falling  = 0;

ISR(TIMER1_CAPT_vect) {
    if (capture_state == 0) {
        time_rising  = Timer1_ICP_Read();
        capture_state = 1;
        Timer1_ICP_SetEdge(0);  /* urmatoarea captura pe falling edge */

    } else if (capture_state == 1) {
        time_falling  = Timer1_ICP_Read();
        capture_state = 2;
        Timer1_ICP_SetEdge(1);  /* reset pentru urmatoarea masurare */
    }
}

void HCSR04_Init(uint8_t port, uint8_t pin) {
    GPIO_Init(port, pin, GPIO_OUTPUT);
    GPIO_Write(port, pin, GPIO_LOW);

    Timer1_ICP_Init(8);             /* prescaler=8 → 1 tick = 0.5µs */
    Timer1_ICP_EnableInterrupt();

    capture_state = 0;
    sei();
}

uint16_t HCSR04_Read(uint8_t port, uint8_t pin) {
    capture_state = 0;

    /* Puls TRIG: 10µs HIGH */
    GPIO_Write(port, pin, GPIO_HIGH);
    _delay_us(10);
    GPIO_Write(port, pin, GPIO_LOW);

    /* Asteapta ambele capturi (timeout 30ms) */
    uint32_t t0 = Millis();
    while (capture_state < 2) {
        if ((Millis() - t0) > 30) return 0;  /* timeout */
    }

    /* Calculeaza latimea pulsului (cu overflow handling) */
    uint16_t pulse_ticks = (time_falling >= time_rising)
                         ? (time_falling - time_rising)
                         : (0xFFFF - time_rising) + time_falling;

    /* prescaler=8 → 1 tick=0.5µs → distance = ticks*0.5/58 = ticks/116 */
    uint16_t distance_cm = pulse_ticks / 116;

    return (distance_cm > 400) ? 400 : distance_cm;
}
