#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "drivers/gpio/gpio.h"

/**
 * @file pwm.h
 * @brief Generic Pulse Width Modulation (PWM) Driver.
 * 
 * Accurately controls the PWM duty cycle for supported pins.
 * Automatically manages underlying Timers (Timer1, Timer2).
 */

/**
 * @brief Initialize PWM on a specific pin.
 * 
 * Configures the associated timer for the given pin. 
 * Note: Pins sharing a timer must use compatible frequencies if underlying hardware dictates it.
 * 
 * @param port GPIO Port (GPIO_PORTB, GPIO_PORTD usually).
 * @param pin Pin number (0-7).
 * @param frequency_hz Desired PWM frequency in Hz.
 */
void PWM_Init(uint8_t port, uint8_t pin, uint32_t frequency_hz);

/**
 * @brief Set the output pulse width for a specific pin.
 *
 * Converts the requested pulse width to the correct timer compare value
 * by reading the active prescaler directly from the hardware registers.
 * This provides full timer resolution regardless of PWM frequency.
 *
 * @param port    GPIO Port (GPIO_PORTB, GPIO_PORTD).
 * @param pin     Pin number (0-7).
 * @param pulse_us Desired pulse width in microseconds.
 *                 Timer1 (D9/D10): 500–2500 µs for servo 0°–180°,
 *                   or 0 to (1 000 000 / frequency_hz) for general PWM.
 *                 Timer2 (D3/D11): same range; 8-bit resolution only.
 * @note For servo control (50 Hz) always prefer Timer1 pins (D9, D10).
 *       Timer2 cannot generate exactly 50 Hz (closest ≈ 61 Hz).
 */
void PWM_SetDutyCycle(uint8_t port, uint8_t pin, uint16_t pulse_us);

/**
 * @brief Stop PWM on a specific pin.
 * 
 * Disables the PWM output for that pin.
 * 
 * @param port GPIO Port.
 * @param pin Pin number.
 */
void PWM_Stop(uint8_t port, uint8_t pin);

#endif // PWM_H
