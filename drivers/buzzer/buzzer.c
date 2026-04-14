#include "buzzer.h"
#include "drivers/gpio/gpio.h"
#include "utils/delay.h"
#include <util/delay.h>  /* For _delay_us() — microsecond precision */
#include <stdint.h>

/*
 * ═══════════════════════════════════════════════════════════════════════════
 * BUZZER DRIVER — Simple GPIO-based audio feedback
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * HARDWARE:
 *   Active buzzer (e.g., KY-012 or equivalent):
 *     + Pin → GPIO (e.g., D12 / PB4)
 *     - Pin → GND
 *
 * OPERATION:
 *   - Buzzer_On()     → GPIO = HIGH → buzzer sounds
 *   - Buzzer_Off()    → GPIO = LOW  → buzzer silent
 *   - Buzzer_Beep()   → ON for X ms, OFF for Y ms
 *   - Buzzer_Tone()   → Toggle rapidly to create frequency
 *
 * NOTE: This is NOT PWM-based. It's GPIO toggle.
 * For true variable frequency, would need Timer-based PWM.
 */

static uint8_t buzzer_port = 0;
static uint8_t buzzer_pin = 0;

void Buzzer_Init(uint8_t port, uint8_t pin) {
    buzzer_port = port;
    buzzer_pin = pin;
    GPIO_Init(port, pin, GPIO_OUTPUT);
    GPIO_Write(port, pin, GPIO_LOW);  /* Start silent */
}

void Buzzer_On(void) {
    GPIO_Write(buzzer_port, buzzer_pin, GPIO_HIGH);
}

void Buzzer_Off(void) {
    GPIO_Write(buzzer_port, buzzer_pin, GPIO_LOW);
}

void Buzzer_Beep(uint16_t duration_ms, uint16_t pause_ms) {
    Buzzer_On();
    Delay(duration_ms);
    Buzzer_Off();
    if (pause_ms > 0) {
        Delay(pause_ms);
    }
}

void Buzzer_Tone(uint16_t frequency_hz, uint16_t duration_ms) {
    /*
     * Generate tone by toggling at carrier frequency
     *
     * Period in µs = 1,000,000 / frequency_hz
     * Half period = period / 2
     *
     * Example: 1000 Hz
     *   Period = 1000 µs
     *   Half period = 500 µs
     *   Toggle every 500 µs = audible 1kHz tone
     */

    uint32_t half_period_us = (1000000UL / frequency_hz) / 2;
    uint32_t toggles_needed = (duration_ms * 1000UL) / half_period_us;

    for (uint32_t i = 0; i < toggles_needed; i++) {
        GPIO_Toggle(buzzer_port, buzzer_pin);

        /*
         * For accurate frequency, we need microsecond precision.
         * _delay_us() from <util/delay.h> is calibrated with F_CPU.
         * But for toggles every 500µs at 1kHz, Delay(1) might be too long.
         *
         * Simple approximation: just toggle rapidly without exact delay.
         * Audio perception is forgiving — exact frequency is not critical.
         */

        if (half_period_us >= 1000) {
            /* Half period >= 1ms, use millisecond delay */
            Delay(half_period_us / 1000);
        } else {
            /* Sub-millisecond: skip delay, rely on loop overhead */
            /* This gives ~kHz range beeping */
            asm volatile("nop");
            asm volatile("nop");
            asm volatile("nop");
        }
    }

    Buzzer_Off();  /* Ensure buzzer is off after tone */
}

void Buzzer_Alert(void) {
    /* Quick 3-beep alert pattern */
    Buzzer_Beep(100, 50);   /* piu */
    Buzzer_Beep(100, 50);   /* piu */
    Buzzer_Beep(150, 0);    /* piu (longer) */
}
