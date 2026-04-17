#include "timer1.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void Timer1_FastPWM_Init(uint16_t prescaler, uint16_t top) {
    // 1. Set Fast PWM Mode 14 (WGM13:0 = 1110)
    // TOP is ICR1, Update of OCR1x at BOTTOM, TOV1 Flag Set on TOP
    
    // TCCR1A: WGM11 (1), WGM10 (0)
    // Preserve existing COM bits if any
    TCCR1A = (TCCR1A & 0xF0) | (1 << WGM11);
    
    // TCCR1B: WGM13 (1), WGM12 (1), CS1x (Prescaler)
    TCCR1B = (1 << WGM13) | (1 << WGM12);

    // Set TOP value
    ICR1 = top;

    // Set Prescaler
    // 001 = /1, 010 = /8, 011 = /64, 100 = /256, 101 = /1024
    switch (prescaler) {
        case 1: TCCR1B |= (1 << CS10); break;
        case 8: TCCR1B |= (1 << CS11); break;
        case 64: TCCR1B |= (1 << CS11) | (1 << CS10); break;
        case 256: TCCR1B |= (1 << CS12); break;
        case 1024: TCCR1B |= (1 << CS12) | (1 << CS10); break;
        default: TCCR1B |= (1 << CS10); // Default to no prescaling
    }

    // Only reset channels not yet in use — avoids disturbing an active PWM output
    if (!(TCCR1A & (1 << COM1A1))) OCR1A = 0;
    if (!(TCCR1A & (1 << COM1B1))) OCR1B = 0;
}

void Timer1_EnableOC1A(void) {
    TCCR1A |= (1 << COM1A1);
    DDRB |= (1 << DDB1);
}

void Timer1_EnableOC1B(void) {
    TCCR1A |= (1 << COM1B1);
    DDRB |= (1 << DDB2);
}

void Timer1_DisableOC1A(void) {
    TCCR1A &= ~(1 << COM1A1);
}

void Timer1_DisableOC1B(void) {
    TCCR1A &= ~(1 << COM1B1);
}

void Timer1_SetDutyCycleA(uint16_t ticks) {
    uint8_t sreg_save = SREG;
    cli();
    if (ticks > ICR1) ticks = ICR1;
    OCR1A = ticks;
    SREG = sreg_save;
}

void Timer1_SetDutyCycleB(uint16_t duty) {
    if (duty > ICR1) duty = ICR1; // Clamp to TOP
    OCR1B = duty;
}

void Timer1_Stop(void) {
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

/* ── Input Capture ────────────────────────────────────────────────────────── */

void Timer1_ICP_Init(uint16_t prescaler) {
    TCCR1A = 0x00;      /* Normal mode — no PWM output */
    TCCR1B = (1 << 6);  /* ICES1=1: capture on rising edge first */
    TCNT1  = 0;

    switch (prescaler) {
        case 1:    TCCR1B |= (1 << CS10); break;
        case 8:    TCCR1B |= (1 << CS11); break;
        case 64:   TCCR1B |= (1 << CS11) | (1 << CS10); break;
        case 256:  TCCR1B |= (1 << CS12); break;
        case 1024: TCCR1B |= (1 << CS12) | (1 << CS10); break;
        default:   TCCR1B |= (1 << CS11); break; /* fallback: prescaler=8 */
    }
}

void Timer1_ICP_SetEdge(uint8_t rising) {
    if (rising) TCCR1B |=  (1 << 6);  /* ICES1=1 → rising edge  */
    else        TCCR1B &= ~(1 << 6);  /* ICES1=0 → falling edge */
}

void Timer1_ICP_EnableInterrupt(void) {
    TIMSK1 |= (1 << 5);   /* ICIE1=1 */
}

void Timer1_ICP_DisableInterrupt(void) {
    TIMSK1 &= ~(1 << 5);  /* ICIE1=0 */
}

uint16_t Timer1_ICP_Read(void) {
    return ICR1;
}

uint16_t Timer1_GetCount(void) {
    return TCNT1;
}

void Timer1_Normal_Init(uint16_t prescaler) {
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1  = 0;
    switch (prescaler) {
        case 1:    TCCR1B = (1 << CS10); break;
        case 8:    TCCR1B = (1 << CS11); break;
        case 64:   TCCR1B = (1 << CS11) | (1 << CS10); break;
        case 256:  TCCR1B = (1 << CS12); break;
        case 1024: TCCR1B = (1 << CS12) | (1 << CS10); break;
        default:   TCCR1B = (1 << CS11); break;
    }
}
