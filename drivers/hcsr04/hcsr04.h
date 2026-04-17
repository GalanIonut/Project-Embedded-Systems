#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>
#include "drivers/gpio/gpio.h"

/**
 * @file hcsr04.h
 * @brief HC-SR04 Ultrasonic Sensor Driver — Dual sensor via PCINT
 *
 * Supports 2 sensors simultaneously. ECHO can be on ANY digital pin.
 * Timer1 is used as time source (TCNT1) — compatible with Servo PWM.
 *
 * Wiring:
 *   TRIG → any GPIO output pin
 *   ECHO → any GPIO input pin  (PCINT detects the edges)
 *   VCC  → 5V,  GND → common GND
 *
 * Usage (1 sensor):
 *   HCSR04_Init(0, GPIO_PORTD, 4, GPIO_PORTD, 5);
 *   uint16_t d = HCSR04_Read(0);
 *
 * Usage (2 sensors):
 *   HCSR04_Init(0, GPIO_PORTD, 4, GPIO_PORTD, 5);
 *   HCSR04_Init(1, GPIO_PORTD, 6, GPIO_PORTD, 7);
 *   uint16_t d0 = HCSR04_Read(0);
 *   uint16_t d1 = HCSR04_Read(1);
 */

#define HCSR04_MAX_SENSORS  2
#define HCSR04_TIMEOUT_MS   30   /* 30ms ≈ 400cm maxim */

/**
 * @brief Initializes an HC-SR04 sensor.
 *
 * @param id         Sensor index: 0 or 1
 * @param trig_port  TRIG port (GPIO_PORTB/C/D)
 * @param trig_pin   TRIG pin (0–7)
 * @param echo_port  ECHO port (GPIO_PORTB/C/D)
 * @param echo_pin   ECHO pin (0–7)
 */
void HCSR04_Init(uint8_t id,
                 uint8_t trig_port, uint8_t trig_pin,
                 uint8_t echo_port, uint8_t echo_pin);

/**
 * @brief Triggers a measurement and returns the distance in cm.
 *
 * Blocking call — waits for the ECHO pulse (max HCSR04_TIMEOUT_MS ms).
 *
 * @param id  Sensor index: 0 or 1
 * @return    Distance in cm (1–400), or 0 on timeout/error
 */
uint16_t HCSR04_Read(uint8_t id);

#endif // HCSR04_H
