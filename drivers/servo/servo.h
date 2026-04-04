#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

/**
 * @file servo.h
 * @brief SG90 Servo Motor Driver.
 *
 * Controls the SG90 servo via Timer1 OC1A (Pin D9 / PB1).
 *
 * Signal specs:
 *   - Frequency  : 50 Hz (20 ms period)
 *   - 0 degrees  : 0.5 ms pulse
 *   - 90 degrees : 1.5 ms pulse
 *   - 180 degrees: 2.5 ms pulse
 *
 * Timer1 configuration (F_CPU = 16 MHz, prescaler = 8):
 *   ICR1 = 39999  (TOP, gives exactly 50 Hz)
 *   Resolution: 0.5 µs per tick  →  ticks = pulse_µs × 2
 */

/**
 * @brief Initialize the servo on pin D9 (PB1 / OC1A) at 50 Hz.
 */
void Servo_Init(void);

/**
 * @brief Move the servo to the specified angle.
 *
 * @param angle Target angle in degrees (0–180).
 *              Values outside this range are clamped.
 */
void Servo_SetAngle(uint8_t angle);

/**
 * @brief Stop the servo PWM output and disable the timer channel.
 */
void Servo_Stop(void);

#endif // SERVO_H
