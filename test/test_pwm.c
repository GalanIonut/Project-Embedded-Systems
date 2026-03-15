#include <stdio.h>
#include <assert.h>
#include "drivers/timer/timer1.h"
#include "drivers/timer/timer2.h"
#include "test/mocks/avr/io.h"

// External helper to reset registers
void Reset_Registers(void);

void test_timer1_init(void) {
    Reset_Registers();
    printf("Testing Timer1 Init...\n");

    // Initialize with Prescaler 64 and TOP 20000
    Timer1_FastPWM_Init(64, 20000);

    // Verify TCCR1A (WGM11 only initially)
    assert(TCCR1A == (1 << WGM11));

    // Enable Channel A
    Timer1_EnableOC1A();
    assert(TCCR1A & (1 << COM1A1));
    assert(DDRB & (1 << DDB1));

    // Enable Channel B
    Timer1_EnableOC1B();
    assert(TCCR1A & (1 << COM1B1));
    assert(DDRB & (1 << DDB2));

    printf("Timer1 Init Passed.\n");
}

void test_timer1_duty(void) {
    printf("Testing Timer1 Duty Cycle...\n");
    Reset_Registers();
    ICR1 = 1000;

    Timer1_SetDutyCycleA(500);
    assert(OCR1A == 500);

    Timer1_SetDutyCycleA(1500); // Should clamp
    assert(OCR1A == 1000);

    printf("Timer1 Duty Cycle Passed.\n");
}

void test_timer2_init(void) {
    Reset_Registers();
    printf("Testing Timer2 Init...\n");

    // Initialize with Prescaler 64
    Timer2_FastPWM_Init(64);

    // Verify TCCR2A (WGM21 | WGM20 initially)
    assert(TCCR2A == ((1 << WGM21) | (1 << WGM20)));

    // Enable Channel A
    Timer2_EnableOC2A();
    assert(TCCR2A & (1 << COM2A1));
    assert(DDRB & (1 << DDB3));

    // Enable Channel B
    Timer2_EnableOC2B();
    assert(TCCR2A & (1 << COM2B1));
    assert(DDRD & (1 << DDD3));

    printf("Timer2 Init Passed.\n");
}

int main(void) {
    test_timer1_init();
    test_timer1_duty();
    test_timer2_init();
    
    printf("All PWM Tests Passed!\n");
    return 0;
}
