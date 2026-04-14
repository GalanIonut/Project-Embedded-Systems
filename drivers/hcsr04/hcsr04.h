#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>
#include "drivers/gpio/gpio.h"

/**
 * @file hcsr04.h
 * @brief HC-SR04 Ultrasonic Distance Sensor Driver
 *
 * Uses Timer1 Input Capture (ICP1 on D8 / PB0) to measure ECHO pulse width.
 * Converts pulse duration to distance in centimeters.
 *
 * Pinout (per sensor):
 *   VCC   → 5V
 *   GND   → GND
 *   TRIG  → any GPIO pin (configured at init)
 *   ECHO  → Timer1 ICP pin (D8 / PB0)
 *
 * Operation:
 *   1. HCSR04_Init(trig_port, trig_pin) — configure TRIG as output
 *   2. HCSR04_Read(trig_port, trig_pin)  — send 10µs pulse, measure echo
 *   3. return distance in cm (0–400)
 */

/**
 * @brief Initialize HC-SR04 sensor by configuring TRIG pin as output.
 *
 * @param port GPIO port of TRIG pin
 * @param pin  GPIO pin number of TRIG pin
 * @note ECHO must be wired to D8 (PB0 / ICP1) — hardware requirement
 */
void HCSR04_Init(uint8_t port, uint8_t pin);

/**
 * @brief Measure and return distance to object in centimeters.
 *
 * Sequence:
 *   1. Send 10 µs HIGH pulse on TRIG
 *   2. Wait for ECHO to go HIGH (rising edge)
 *   3. Measure time until ECHO goes LOW (falling edge) using Timer1 ICP
 *   4. Convert pulse width in µs to distance in cm using: distance = echo_us / 58
 *
 * @param port GPIO port of TRIG pin
 * @param pin  GPIO pin number of TRIG pin
 * @return Distance in cm (uint16_t, 0–400 typical range)
 * @note Maximum measurement: ~23ms → ~400cm (sensor spec)
 * @note BLOCKING function (~60ms worst case including Timeout)
 * @note If no echo received within timeout (30ms), returns 0
 */
uint16_t HCSR04_Read(uint8_t port, uint8_t pin);

#endif // HCSR04_H
