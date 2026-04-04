#include "servo.h"
#include "drivers/pwm/pwm.h"
#include "drivers/gpio/gpio.h"

/*
 * SG90 Servo — Pin D9 (PB1 / OC1A / Timer1)
 *
 * Timer1 at 50 Hz, prescaler = 8, F_CPU = 16 MHz:
 *   ICR1 = 39 999    (TOP → exactly 50 Hz)
 *   1 tick = 0.5 µs
 *
 *   0°   → 500 µs pulse
 *   90°  → 1500 µs pulse
 *   180° → 2500 µs pulse
 *
 * PWM_SetDutyCycle now accepts pulse width in µs directly,
 * so no raw timer register access is needed here.
 */

#define SERVO_PULSE_MIN_US  500U   /* 0°   */
#define SERVO_PULSE_MAX_US  2500U  /* 180° */

void Servo_Init(void) {
    PWM_Init(GPIO_PORTB, 1, 50);
    Servo_SetAngle(90);
}

void Servo_SetAngle(uint8_t angle) {
    if (angle > 180) {
        angle = 180;
    }

    /*
     * Linear map: [0°, 180°] → [500 µs, 2500 µs]
     *   pulse_us = 500 + angle × 2000 / 180
     */
    uint16_t pulse_us = SERVO_PULSE_MIN_US +
                        ((uint32_t)angle * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)) / 180U;

    PWM_SetDutyCycle(GPIO_PORTB, 1, pulse_us);
}

void Servo_Stop(void) {
    PWM_Stop(GPIO_PORTB, 1);
}
