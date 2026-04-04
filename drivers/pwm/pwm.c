#include "pwm.h"
#include "drivers/timer/timer1.h"
#include "drivers/timer/timer2.h"
#include "bsp/bsp.h"
#include <avr/io.h>

// Timer1: D9 (PB1 / OC1A), D10 (PB2 / OC1B)  — 16-bit, Fast PWM Mode 14
// Timer2: D11 (PB3 / OC2A), D3 (PD3 / OC2B)  — 8-bit,  Fast PWM Mode 3

/* -------------------------------------------------------------------------
 * Internal helpers — read active prescaler from hardware registers.
 * This lets PWM_SetDutyCycle convert µs → ticks without storing extra state.
 * ------------------------------------------------------------------------- */

static uint16_t pwm_timer1_prescaler(void) {
    switch (TCCR1B & 0x07) {
        case 1:  return 1;
        case 2:  return 8;
        case 3:  return 64;
        case 4:  return 256;
        case 5:  return 1024;
        default: return 1;
    }
}

static uint16_t pwm_timer2_prescaler(void) {
    switch (TCCR2B & 0x07) {
        case 1:  return 1;
        case 2:  return 8;
        case 3:  return 32;
        case 4:  return 64;
        case 5:  return 128;
        case 6:  return 256;
        case 7:  return 1024;
        default: return 1;
    }
}

/* -------------------------------------------------------------------------
 * PWM_Init
 * ------------------------------------------------------------------------- */
void PWM_Init(uint8_t port, uint8_t pin, uint32_t frequency_hz) {
    if (port == GPIO_PORTB) {
        if (pin == 1 || pin == 2) {
            // Timer1 (16-bit) — pick smallest prescaler giving TOP < 65536
            const uint32_t f_cpu = F_CPU;
            const uint16_t prescalers[] = {1, 8, 64, 256, 1024};
            uint16_t chosen_prescaler = 0;
            uint32_t top = 0;

            for (int i = 0; i < 5; i++) {
                uint32_t temp_top = f_cpu / ((uint32_t)prescalers[i] * frequency_hz) - 1;
                if (temp_top < 65536UL) {
                    chosen_prescaler = prescalers[i];
                    top = temp_top;
                    break;
                }
            }

            if (chosen_prescaler > 0) {
                Timer1_FastPWM_Init(chosen_prescaler, (uint16_t)top);
                if (pin == 1) Timer1_EnableOC1A();
                else          Timer1_EnableOC1B();
            }

        } else if (pin == 3) {
            // Timer2 (8-bit / OC2A / D11) — best-fit prescaler
            const uint32_t f_cpu = F_CPU;
            const uint16_t prescalers[] = {1, 8, 32, 64, 128, 256, 1024};
            uint16_t best_prescaler = 1024;
            uint32_t min_diff = 0xFFFFFFFFUL;

            for (int i = 0; i < 7; i++) {
                uint32_t actual_freq = f_cpu / ((uint32_t)prescalers[i] * 256UL);
                uint32_t diff = (actual_freq > frequency_hz)
                              ? (actual_freq - frequency_hz)
                              : (frequency_hz - actual_freq);
                if (diff < min_diff) {
                    min_diff = diff;
                    best_prescaler = prescalers[i];
                }
            }
            Timer2_FastPWM_Init(best_prescaler);
            Timer2_EnableOC2A();
        }

    } else if (port == GPIO_PORTD) {
        if (pin == 3) {
            // Timer2 (8-bit / OC2B / D3) — best-fit prescaler
            const uint32_t f_cpu = F_CPU;
            const uint16_t prescalers[] = {1, 8, 32, 64, 128, 256, 1024};
            uint16_t best_prescaler = 1024;
            uint32_t min_diff = 0xFFFFFFFFUL;

            for (int i = 0; i < 7; i++) {
                uint32_t actual_freq = f_cpu / ((uint32_t)prescalers[i] * 256UL);
                uint32_t diff = (actual_freq > frequency_hz)
                              ? (actual_freq - frequency_hz)
                              : (frequency_hz - actual_freq);
                if (diff < min_diff) {
                    min_diff = diff;
                    best_prescaler = prescalers[i];
                }
            }
            Timer2_FastPWM_Init(best_prescaler);
            Timer2_EnableOC2B();
        }
    }
}

/* -------------------------------------------------------------------------
 * PWM_SetDutyCycle
 *
 * Timer1 (16-bit):
 *   ticks = pulse_us × (F_CPU / 1 000 000) / prescaler
 *   Example 50 Hz, prescaler=8:  ticks = pulse_us × 2
 *   → 500 µs = 1 000 ticks (0°),  2 500 µs = 5 000 ticks (180°)
 *   Full range: 0 – ICR1 (39 999) → sub-microsecond resolution
 *
 * Timer2 (8-bit, TOP = 255):
 *   OCR = pulse_us × 255 × (F_CPU / 1 000 000) / (prescaler × 256)
 *   Limited to 8-bit resolution; 50 Hz not achievable exactly.
 * ------------------------------------------------------------------------- */
void PWM_SetDutyCycle(uint8_t port, uint8_t pin, uint16_t pulse_us) {
    if (port == GPIO_PORTB) {

        if (pin == 1 || pin == 2) {
            // Timer1 — full 16-bit resolution
            uint16_t prescaler = pwm_timer1_prescaler();
            uint16_t ticks = (uint16_t)((uint32_t)pulse_us
                             * (F_CPU / 1000000UL) / prescaler);

            if (pin == 1) Timer1_SetDutyCycleA(ticks);
            else          Timer1_SetDutyCycleB(ticks);

        } else if (pin == 3) {
            // Timer2 OC2A (D11) — 8-bit resolution
            uint16_t prescaler = pwm_timer2_prescaler();
            uint8_t ocr = (uint8_t)((uint32_t)pulse_us * 255UL
                          * (F_CPU / 1000000UL) / ((uint32_t)prescaler * 256UL));
            Timer2_SetDutyCycleA(ocr);
        }

    } else if (port == GPIO_PORTD) {

        if (pin == 3) {
            // Timer2 OC2B (D3) — 8-bit resolution
            uint16_t prescaler = pwm_timer2_prescaler();
            uint8_t ocr = (uint8_t)((uint32_t)pulse_us * 255UL
                          * (F_CPU / 1000000UL) / ((uint32_t)prescaler * 256UL));
            Timer2_SetDutyCycleB(ocr);
        }
    }
}

/* -------------------------------------------------------------------------
 * PWM_Stop
 * ------------------------------------------------------------------------- */
void PWM_Stop(uint8_t port, uint8_t pin) {
    if (port == GPIO_PORTB) {
        if (pin == 1) {
            Timer1_DisableOC1A();
            if (!(TCCR1A & ((1 << COM1A1) | (1 << COM1B1)))) Timer1_Stop();
        } else if (pin == 2) {
            Timer1_DisableOC1B();
            if (!(TCCR1A & ((1 << COM1A1) | (1 << COM1B1)))) Timer1_Stop();
        } else if (pin == 3) {
            Timer2_DisableOC2A();
            if (!(TCCR2A & ((1 << COM2A1) | (1 << COM2B1)))) Timer2_Stop();
        }
    } else if (port == GPIO_PORTD) {
        if (pin == 3) {
            Timer2_DisableOC2B();
            if (!(TCCR2A & ((1 << COM2A1) | (1 << COM2B1)))) Timer2_Stop();
        }
    }
}
