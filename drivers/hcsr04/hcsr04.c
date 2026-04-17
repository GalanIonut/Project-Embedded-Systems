#include "hcsr04.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timer/timer0.h"
#include "drivers/timer/timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * HC-SR04 DRIVER — Dual sensor via Pin Change Interrupt (PCINT)
 *
 * Why PCINT and not ICP?
 *   - ICP is fixed on D8 — only one sensor possible
 *   - PCINT is available on all ATmega328P pins
 *   - Timer1 remains free for Servo PWM
 *
 * Timer1 dual-use:
 *   - Servo PWM:  Timer1 in Fast PWM mode, OC1A/OC1B controls the angle
 *   - HC-SR04:    PCINT reads TCNT1 on each edge — no interference
 *
 * Overflow handling:
 *   Normal mode:   TOP = 0xFFFF
 *   Fast PWM M14:  TOP = ICR1
 *   Formula:  if (t_fall < t_rise) → pulse = (TOP - t_rise) + t_fall + 1
 */

typedef struct {
    uint8_t trig_port, trig_pin;
    uint8_t echo_port, echo_pin;
    volatile uint8_t  state;      /* 0=idle, 1=rising captured, 2=done */
    volatile uint16_t t_rising;
    volatile uint16_t t_falling;
    uint8_t           last_echo;
} HCSR04_Sensor_t;

static HCSR04_Sensor_t sensors[HCSR04_MAX_SENSORS];
static uint8_t         num_sensors = 0;

/* ── Overflow handling ───────────────────────────────────────── */

static uint16_t calc_ticks(uint16_t t_rise, uint16_t t_fall) {
    if (t_fall >= t_rise) return t_fall - t_rise;
    /* TCNT1 overflowed: detect whether we are in PWM mode (ICR1) or Normal mode (0xFFFF) */
    uint16_t top = (TCCR1A & (1 << WGM11)) ? ICR1 : 0xFFFF;
    return (top - t_rise) + t_fall + 1;
}

/* ── PCINT setup ─────────────────────────────────────────────── */

static void pcint_enable(uint8_t port, uint8_t pin) {
    switch (port) {
        case GPIO_PORTB: PCICR |= (1<<PCIE0); PCMSK0 |= (1<<pin); break;
        case GPIO_PORTC: PCICR |= (1<<PCIE1); PCMSK1 |= (1<<pin); break;
        case GPIO_PORTD: PCICR |= (1<<PCIE2); PCMSK2 |= (1<<pin); break;
        default: break;
    }
}

/* ── Common PCINT handler ────────────────────────────────────── */

static void pcint_handler(uint8_t port) {
    uint16_t now = Timer1_GetCount();

    for (uint8_t i = 0; i < num_sensors; i++) {
        if (sensors[i].echo_port != port) continue;

        uint8_t cur = GPIO_Read(sensors[i].echo_port, sensors[i].echo_pin);
        if (cur == sensors[i].last_echo) continue;
        sensors[i].last_echo = cur;

        if (cur == GPIO_HIGH && sensors[i].state == 0) {
            sensors[i].t_rising = now;
            sensors[i].state = 1;
        } else if (cur == GPIO_LOW && sensors[i].state == 1) {
            sensors[i].t_falling = now;
            sensors[i].state = 2;
        }
    }
}

ISR(PCINT0_vect) { pcint_handler(GPIO_PORTB); }
ISR(PCINT1_vect) { pcint_handler(GPIO_PORTC); }
ISR(PCINT2_vect) { pcint_handler(GPIO_PORTD); }

/* ── API ─────────────────────────────────────────────────────── */

void HCSR04_Init(uint8_t id,
                 uint8_t trig_port, uint8_t trig_pin,
                 uint8_t echo_port, uint8_t echo_pin)
{
    if (id >= HCSR04_MAX_SENSORS) return;

    sensors[id].trig_port = trig_port;
    sensors[id].trig_pin  = trig_pin;
    sensors[id].echo_port = echo_port;
    sensors[id].echo_pin  = echo_pin;
    sensors[id].state     = 0;
    sensors[id].last_echo = GPIO_LOW;

    GPIO_Init(trig_port, trig_pin, GPIO_OUTPUT);
    GPIO_Write(trig_port, trig_pin, GPIO_LOW);
    GPIO_Init(echo_port, echo_pin, GPIO_INPUT);

    pcint_enable(echo_port, echo_pin);

    if (id + 1 > num_sensors) num_sensors = id + 1;

    /* Start Timer1 in Normal mode if not already running (e.g. without Servo) */
    if (!(TCCR1B & 0x07)) Timer1_Normal_Init(8);

    sei();
}

uint16_t HCSR04_Read(uint8_t id) {
    if (id >= num_sensors) return 0;

    sensors[id].state = 0;

    GPIO_Write(sensors[id].trig_port, sensors[id].trig_pin, GPIO_HIGH);
    _delay_us(10);
    GPIO_Write(sensors[id].trig_port, sensors[id].trig_pin, GPIO_LOW);

    uint32_t t0 = Millis();
    while (sensors[id].state < 2) {
        if ((Millis() - t0) > HCSR04_TIMEOUT_MS) return 0;
    }

    uint16_t ticks = calc_ticks(sensors[id].t_rising, sensors[id].t_falling);
    uint16_t dist  = ticks / 116;  /* prescaler=8: 1 tick=0.5µs → dist=ticks/116 */
    return (dist > 400) ? 400 : dist;
}
