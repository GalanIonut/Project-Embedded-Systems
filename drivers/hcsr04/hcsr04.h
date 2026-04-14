#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>
#include "drivers/gpio/gpio.h"

/**
 * @file hcsr04.h
 * @brief HC-SR04 Ultrasonic Sensor Driver — Dual sensor via PCINT
 *
 * Suporta 2 senzori simultan. ECHO poate fi pe ORICE pin digital.
 * Timer1 e folosit ca sursa de timp (TCNT1) — compatibil cu Servo PWM.
 *
 * Cablaj:
 *   TRIG → orice pin GPIO output
 *   ECHO → orice pin GPIO input  (PCINT detecteaza edge-urile)
 *   VCC  → 5V,  GND → GND comun
 *
 * Utilizare (1 senzor):
 *   HCSR04_Init(0, GPIO_PORTD, 4, GPIO_PORTD, 5);
 *   uint16_t d = HCSR04_Read(0);
 *
 * Utilizare (2 senzori):
 *   HCSR04_Init(0, GPIO_PORTD, 4, GPIO_PORTD, 5);
 *   HCSR04_Init(1, GPIO_PORTD, 6, GPIO_PORTD, 7);
 *   uint16_t d0 = HCSR04_Read(0);
 *   uint16_t d1 = HCSR04_Read(1);
 */

#define HCSR04_MAX_SENSORS  2
#define HCSR04_TIMEOUT_MS   30   /* 30ms ≈ 400cm maxim */

/**
 * @brief Initializeaza un senzor HC-SR04.
 *
 * @param id         Index senzor: 0 sau 1
 * @param trig_port  Port TRIG (GPIO_PORTB/C/D)
 * @param trig_pin   Pin TRIG (0–7)
 * @param echo_port  Port ECHO (GPIO_PORTB/C/D)
 * @param echo_pin   Pin ECHO (0–7)
 */
void HCSR04_Init(uint8_t id,
                 uint8_t trig_port, uint8_t trig_pin,
                 uint8_t echo_port, uint8_t echo_pin);

/**
 * @brief Declanseaza masurare si returneaza distanta in cm.
 *
 * Apel blocant — asteapta pulsul ECHO (max HCSR04_TIMEOUT_MS ms).
 *
 * @param id  Index senzor: 0 sau 1
 * @return    Distanta in cm (1–400), sau 0 la timeout/eroare
 */
uint16_t HCSR04_Read(uint8_t id);

#endif // HCSR04_H
