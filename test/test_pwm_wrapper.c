#include <stdio.h>
#include <assert.h>
#include "drivers/pwm/pwm.h"
#include "drivers/timer/timer1.h"
#include "drivers/timer/timer2.h"
#include "test/mocks/avr/io.h"
#include "bsp/uno.h"

// External helper to reset registers
void Reset_Registers(void);

void test_pwm_init_timer1(void) {
    Reset_Registers();
    printf("Testing PWM Init (Timer1 / D9)...\n");

    // Initialize Timer1 on D9 (PB1) for 50Hz
    // 50Hz
    // Algorithm:
    // P=1: TOP = 16M/50 - 1 = 319999 (Too big)
    // P=8: TOP = 2M/50 - 1 = 39999 (Fits!)
    // Should choose Prescaler 8.
    
    PWM_Init(GPIO_PORTB, 1, 50);

    // Verify Prescaler 8 (CS11) = 2
    // TCCR1B & 0x7 should be 2
    assert((TCCR1B & 0x07) == 2);

    // Verify TOP (ICR1)
    // 39999
    assert(ICR1 == 39999);

    printf("PWM Init Timer1 Passed.\n");
}

void test_pwm_init_timer2(void) {
    Reset_Registers();
    printf("Testing PWM Init (Timer2 / D11)...\n");

    // Initialize Timer2 on D11 (PB3) for ~1kHz
    // Timer2 is 8-bit, fixed TOP=255.
    // Freq = 16MHz / (Prescaler * 256)
    // 1000 = 16000000 / (P * 256)
    // P = 16000000 / 256000 = 62.5 -> Closest is 64
    PWM_Init(GPIO_PORTB, 3, 1000);

    // Verify Prescaler 64 (CS22) = 4 (1<<2)
    assert((TCCR2B & 0x07) == 4);

    printf("PWM Init Timer2 Passed.\n");
}

void test_pwm_duty_cycle(void) {
    Reset_Registers();
    printf("Testing PWM Duty Cycle...\n");

    // PWM_SetDutyCycle takes pulse_us (microseconds), not a 0-255 byte.
    // With F_CPU = 16 MHz and prescaler = 8:
    //   ticks = pulse_us * (16000000 / 1000000) / 8 = pulse_us * 2

    // Set TCCR1B to prescaler 8 (CS11 = bit 1) so pwm_timer1_prescaler() returns 8.
    TCCR1B = (1 << CS11);
    ICR1 = 39999; // TOP for 50 Hz @ prescaler 8

    // 500 us (0 degrees) -> 500 * 2 = 1000 ticks
    PWM_SetDutyCycle(GPIO_PORTB, 1, 500);
    assert(OCR1A == 1000);

    // 1500 us (90 degrees, neutral) -> 1500 * 2 = 3000 ticks
    PWM_SetDutyCycle(GPIO_PORTB, 1, 1500);
    assert(OCR1A == 3000);

    // 2500 us (180 degrees) -> 2500 * 2 = 5000 ticks
    PWM_SetDutyCycle(GPIO_PORTB, 1, 2500);
    assert(OCR1A == 5000);

    // Clamping: pulse wider than period -> clamped to ICR1
    ICR1 = 1000;
    PWM_SetDutyCycle(GPIO_PORTB, 1, 1000); // 1000 * 2 = 2000 > ICR1=1000 -> clamped
    assert(OCR1A == 1000);

    printf("PWM Duty Cycle Passed.\n");
}

void test_pwm_stop(void) {
    Reset_Registers();
    printf("Testing PWM Stop...\n");

    // Start PWM on Timer1 (D9)
    PWM_Init(GPIO_PORTB, 1, 50);
    assert((TCCR1B & 0x07) != 0); // Timer is running

    // Stop PWM
    PWM_Stop(GPIO_PORTB, 1);
    assert((TCCR1B & 0x07) == 0); // Timer is stopped

    // Start PWM on Timer2 (D11)
    PWM_Init(GPIO_PORTB, 3, 1000);
    assert((TCCR2B & 0x07) != 0); // Timer is running

    // Stop PWM
    PWM_Stop(GPIO_PORTB, 3);
    assert((TCCR2B & 0x07) == 0); // Timer is stopped

    printf("PWM Stop Passed.\n");
}

void test_pwm_shared_timer(void) {
    Reset_Registers();
    printf("Testing PWM Shared Timer (Timer1)... \n");

    // Init PWM on D9 and D10 (Both Timer1)
    PWM_Init(GPIO_PORTB, 1, 50);
    PWM_Init(GPIO_PORTB, 2, 50);
    assert((TCCR1B & 0x07) != 0); // Timer is running
    assert((TCCR1A & (1 << COM1A1)) != 0); // OC1A connected
    assert((TCCR1A & (1 << COM1B1)) != 0); // OC1B connected

    // Stop PWM on D9
    PWM_Stop(GPIO_PORTB, 1);

    // Current implementation STOPS THE WHOLE TIMER, so this will fail if we want it to stay running.
    // Let's see if it currently fails or if we want to change behavior.
    // For now, I'll expect it to stay running if I improve PWM_Stop.
    // If it stays as is, the next line will fail.
    assert((TCCR1B & 0x07) != 0); // Timer SHOULD STILL BE RUNNING
    assert((TCCR1A & (1 << COM1A1)) == 0); // OC1A SHOULD BE DISCONNECTED
    assert((TCCR1A & (1 << COM1B1)) != 0); // OC1B SHOULD STILL BE CONNECTED

    printf("PWM Shared Timer Passed.\n");
}

void test_pwm_shared_timer2(void) {
    Reset_Registers();
    printf("Testing PWM Shared Timer (Timer2)... \n");

    // Init PWM on D11 (PB3) and D3 (PD3) (Both Timer2)
    PWM_Init(GPIO_PORTB, 3, 1000);
    PWM_Init(GPIO_PORTD, 3, 1000);
    assert((TCCR2B & 0x07) != 0); // Timer is running
    assert((TCCR2A & (1 << COM2A1)) != 0); // OC2A connected
    assert((TCCR2A & (1 << COM2B1)) != 0); // OC2B connected

    // Stop PWM on D11
    PWM_Stop(GPIO_PORTB, 3);
    assert((TCCR2B & 0x07) != 0); // Timer SHOULD STILL BE RUNNING
    assert((TCCR2A & (1 << COM2A1)) == 0); // OC2A SHOULD BE DISCONNECTED
    assert((TCCR2A & (1 << COM2B1)) != 0); // OC2B SHOULD STILL BE CONNECTED

    // Stop PWM on D3
    PWM_Stop(GPIO_PORTD, 3);
    assert((TCCR2B & 0x07) == 0); // Timer SHOULD BE STOPPED NOW

    printf("PWM Shared Timer2 Passed.\n");
}

int main(void) {
    test_pwm_init_timer1();
    test_pwm_init_timer2();
    test_pwm_duty_cycle();
    test_pwm_stop();
    test_pwm_shared_timer();
    test_pwm_shared_timer2();
    
    printf("All PWM Wrapper Tests Passed!\n");
    return 0;
}
