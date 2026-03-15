#include "pwm.h"
#include "drivers/timer/timer1.h"
#include "drivers/timer/timer2.h"
#include "bsp/uno.h"

// Helper to determine Timer and Channel from Port/Pin
// Timer1: D9 (PB1), D10 (PB2)
// Timer2: D3 (PD3), D11 (PB3)

/**
 * @brief Initialize PWM on a specific pin.
 */
void PWM_Init(uint8_t port, uint8_t pin, uint32_t frequency_hz) {
    if (port == GPIO_PORTB) {
        if (pin == 1 || pin == 2) {
            // Timer1 (16-bit)
            uint32_t f_cpu = 16000000UL;
            uint16_t prescalers[] = {1, 8, 64, 256, 1024};
            uint16_t chosen_prescaler = 0;
            uint32_t top = 0;

            for (int i = 0; i < 5; i++) {
                uint32_t temp_top = f_cpu / (prescalers[i] * frequency_hz) - 1;
                if (temp_top < 65536) {
                    chosen_prescaler = prescalers[i];
                    top = (uint16_t)temp_top;
                    break;
                }
            }
            
            if (chosen_prescaler > 0) {
                Timer1_FastPWM_Init(chosen_prescaler, (uint16_t)top);
                if (pin == 1) Timer1_EnableOC1A();
                else Timer1_EnableOC1B();
            }
        } else if (pin == 3) {
            // Timer2 (OC2A / D11)
            uint32_t f_cpu = 16000000UL;
            uint16_t prescalers[] = {1, 8, 32, 64, 128, 256, 1024};
            uint16_t best_prescaler = 1024;
            uint32_t min_diff = 0xFFFFFFFF;

            for (int i = 0; i < 7; i++) {
                uint32_t actual_freq = f_cpu / (prescalers[i] * 256);
                uint32_t diff = (actual_freq > frequency_hz) ? (actual_freq - frequency_hz) : (frequency_hz - actual_freq);
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
            // Timer2 (OC2B / D3)
            uint32_t f_cpu = 16000000UL;
            uint16_t prescalers[] = {1, 8, 32, 64, 128, 256, 1024};
            uint16_t best_prescaler = 1024;
            uint32_t min_diff = 0xFFFFFFFF;

            for (int i = 0; i < 7; i++) {
                uint32_t actual_freq = f_cpu / (prescalers[i] * 256);
                uint32_t diff = (actual_freq > frequency_hz) ? (actual_freq - frequency_hz) : (frequency_hz - actual_freq);
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

void PWM_SetDutyCycle(uint8_t port, uint8_t pin, uint8_t duty) {
    if (port == GPIO_PORTB) {
        if (pin == 1) { // D9 / OC1A
            uint32_t val = ((uint32_t)duty * ICR1) / 255;
            Timer1_SetDutyCycleA((uint16_t)val);
        } else if (pin == 2) { // D10 / OC1B
            uint32_t val = ((uint32_t)duty * ICR1) / 255;
            Timer1_SetDutyCycleB((uint16_t)val);
        } else if (pin == 3) { // D11 / OC2A
            Timer2_SetDutyCycleA(duty);
        }
    } else if (port == GPIO_PORTD) {
        if (pin == 3) { // D3 / OC2B
            Timer2_SetDutyCycleB(duty);
        }
    }
}

void PWM_Stop(uint8_t port, uint8_t pin) {
     if (port == GPIO_PORTB) {
        if (pin == 1) {
             Timer1_DisableOC1A();
             // Stop timer if no more PWM pins are active on Timer1
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
