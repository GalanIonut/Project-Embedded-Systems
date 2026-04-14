#ifndef TIMER1_H
#define TIMER1_H

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief Initialize Timer1 for Fast PWM mode.
 * 
 * Configures Timer1 for Fast PWM (Mode 14), where ICR1 sets the TOP value 
 * (frequency) and OCR1A/OCR1B set the duty cycles.
 * 
 * Frequency = F_CPU / (Prescaler * (1 + TOP))
 * 
 * @param prescaler Prescaler value (1, 8, 64, 256, 1024)
 * @param top TOP value (ICR1) to determine PWM frequency
 */
void Timer1_FastPWM_Init(uint16_t prescaler, uint16_t top);

/**
 * @brief Enable PWM output on Channel A (PB1 / D9).
 */
void Timer1_EnableOC1A(void);

/**
 * @brief Enable PWM output on Channel B (PB2 / D10).
 */
void Timer1_EnableOC1B(void);

/**
 * @brief Disable PWM output on Channel A (PB1 / D9).
 */
void Timer1_DisableOC1A(void);

/**
 * @brief Disable PWM output on Channel B (PB2 / D10).
 */
void Timer1_DisableOC1B(void);

/**
 * @brief Set the duty cycle for Channel A (Pin D9 / PB1).
 * 
 * @param duty val (0 to TOP)
 */
void Timer1_SetDutyCycleA(uint16_t duty);

/**
 * @brief Set the duty cycle for Channel B (Pin D10 / PB2).
 * 
 * @param duty val (0 to TOP)
 */
void Timer1_SetDutyCycleB(uint16_t duty);

/**
 * @brief Stop Timer1.
 */
void Timer1_Stop(void);

/* ── Input Capture (ICP1 / D8 / PB0) ─────────────────────────────────────── */

/**
 * @brief Initialize Timer1 for Input Capture mode.
 *
 * Configures Timer1 in Normal mode (free-running counter).
 * ECHO pin must be connected to D8 (PB0 / ICP1) — hardware fixed.
 *
 * @param prescaler Clock prescaler: 1, 8, 64, 256, 1024
 *                  prescaler=8 → 1 tick = 0.5µs @ 16MHz (recommended for HC-SR04)
 */
void Timer1_ICP_Init(uint16_t prescaler);

/**
 * @brief Select which edge triggers the next Input Capture.
 *
 * @param rising  1 = capture on rising edge (ECHO goes HIGH)
 *                0 = capture on falling edge (ECHO goes LOW)
 */
void Timer1_ICP_SetEdge(uint8_t rising);

/**
 * @brief Enable Timer1 Input Capture interrupt (TIMER1_CAPT_vect).
 */
void Timer1_ICP_EnableInterrupt(void);

/**
 * @brief Disable Timer1 Input Capture interrupt.
 */
void Timer1_ICP_DisableInterrupt(void);

/**
 * @brief Read the last captured timer value from ICR1.
 *
 * @return 16-bit timer value at the moment of the captured edge
 */
uint16_t Timer1_ICP_Read(void);

/**
 * @brief Read current Timer1 counter (TCNT1) — works in any mode.
 *
 * Folosit de HC-SR04 via PCINT pentru a timestamp edge-urile ECHO
 * fara sa interfereze cu Servo PWM pe acelasi timer.
 */
uint16_t Timer1_GetCount(void);

/**
 * @brief Porneste Timer1 in Normal mode (free-running).
 *
 * Apelat de HC-SR04 cand Servo nu e activ si Timer1 nu ruleaza.
 * Daca Servo PWM e activ, NU apela — Timer1 ruleaza deja.
 *
 * @param prescaler  1, 8, 64, 256, 1024
 */
void Timer1_Normal_Init(uint16_t prescaler);

#endif // TIMER1_H
