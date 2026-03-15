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

#endif // TIMER1_H
