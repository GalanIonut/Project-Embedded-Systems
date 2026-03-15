#include "timer2.h"

void Timer2_FastPWM_Init(uint16_t prescaler) {
    // 1. Set Fast PWM Mode 3 (WGM22:0 = 011)
    // TOP is 0xFF, Update of OCR2x at BOTTOM, TOV2 Flag Set on MAX
    
    // TCCR2A: WGM21 (1), WGM20 (1)
    // Preserve existing COM bits if any
    TCCR2A = (TCCR2A & 0xF0) | (1 << WGM21) | (1 << WGM20);
    
    // TCCR2B: WGM22 (0), CS2x (Prescaler)
    // TCCR2B bits for prescaler:
    // 001 = /1, 010 = /8, 011 = /32, 100 = /64, 101 = /128, 110 = /256, 111 = /1024
    
    uint8_t cs_bits = 0;
    switch (prescaler) {
        case 1: cs_bits = (1 << CS20); break;
        case 8: cs_bits = (1 << CS21); break;
        case 32: cs_bits = (1 << CS21) | (1 << CS20); break;
        case 64: cs_bits = (1 << CS22); break;
        case 128: cs_bits = (1 << CS22) | (1 << CS20); break;
        case 256: cs_bits = (1 << CS22) | (1 << CS21); break;
        case 1024: cs_bits = (1 << CS22) | (1 << CS21) | (1 << CS20); break;
        default: cs_bits = (1 << CS22) | (1 << CS21) | (1 << CS20); // Default to /1024
    }
    
    TCCR2B = cs_bits;

    // Initialize Duty Cycles to 0 if not in use
    if (!(TCCR2A & (1 << COM2A1))) OCR2A = 0;
    if (!(TCCR2A & (1 << COM2B1))) OCR2B = 0;
}

void Timer2_EnableOC2A(void) {
    TCCR2A |= (1 << COM2A1);
    DDRB |= (1 << DDB3); // PB3 as Output (D11)
}

void Timer2_EnableOC2B(void) {
    TCCR2A |= (1 << COM2B1);
    DDRD |= (1 << DDD3); // PD3 as Output (D3)
}

void Timer2_DisableOC2A(void) {
    TCCR2A &= ~(1 << COM2A1);
}

void Timer2_DisableOC2B(void) {
    TCCR2A &= ~(1 << COM2B1);
}

void Timer2_SetDutyCycleA(uint8_t duty) {
    OCR2A = duty;
}

void Timer2_SetDutyCycleB(uint8_t duty) {
    OCR2B = duty;
}

void Timer2_Stop(void) {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
}
