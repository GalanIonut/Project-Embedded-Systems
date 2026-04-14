#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include "drivers/gpio/gpio.h"

/**
 * @file buzzer.h
 * @brief Simple Buzzer Driver
 *
 * Controls an active buzzer (piezo speaker) via GPIO pin toggle.
 * PWM frequency ~1000 Hz = audio tone ("piu" sound)
 *
 * Active Buzzer (recommended):
 *   + Pin  (red)      → GPIO pin (defined by user)
 *   - Pin  (black)    → GND
 *   VCC (optional red) → 5V (depends on model)
 *
 * Wiring example:
 *   Buzzer + → D12 (PB4)
 *   Buzzer - → GND
 *
 * Operation:
 *   Buzzer_Init(GPIO_PORTB, 4)     — configure D12 as output
 *   Buzzer_Beep(100, 50)            — 100ms beep, 50ms pause
 *   Buzzer_Tone(1000, 200)          — 1000 Hz tone for 200ms
 *   Buzzer_On() / Buzzer_Off()      — continuous on/off
 */

/**
 * @brief Initialize buzzer pin as GPIO output
 *
 * @param port GPIO port (GPIO_PORTB, GPIO_PORTD, etc)
 * @param pin  GPIO pin number (0-7)
 */
void Buzzer_Init(uint8_t port, uint8_t pin);

/**
 * @brief Turn buzzer ON
 */
void Buzzer_On(void);

/**
 * @brief Turn buzzer OFF
 */
void Buzzer_Off(void);

/**
 * @brief Single beep with configurable duration
 *
 * @param duration_ms    Beep duration in milliseconds
 * @param pause_ms       Pause after beep in milliseconds (0 = no pause)
 *
 * Example:
 *   Buzzer_Beep(100, 50)   — 100ms beep, 50ms silence
 *   Buzzer_Beep(50, 0)     — 50ms beep, no pause
 */
void Buzzer_Beep(uint16_t duration_ms, uint16_t pause_ms);

/**
 * @brief Generate tone at specified frequency
 *
 * Creates a PWM-like beep by toggling GPIO at carrier frequency.
 * Higher frequency = higher pitch.
 *
 * @param frequency_hz   Tone frequency in Hz (500–4000 typical)
 * @param duration_ms    How long to generate tone
 *
 * Example:
 *   Buzzer_Tone(1000, 200)  — 1kHz tone for 200ms
 *   Buzzer_Tone(2000, 100)  — 2kHz tone for 100ms (higher pitch)
 */
void Buzzer_Tone(uint16_t frequency_hz, uint16_t duration_ms);

/**
 * @brief Quick warning beep pattern
 *
 * 3 rapid beeps: "piu-piu-piu" — indicates proximity alert
 */
void Buzzer_Alert(void);

#endif // BUZZER_H
