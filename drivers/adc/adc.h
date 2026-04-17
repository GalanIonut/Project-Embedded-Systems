#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/**
 * @file adc.h
 * @brief ADC driver for ATmega328P.
 *
 * Reference: AVCC (5V). Prescaler: 128 → 125 kHz ADC clock @ 16 MHz.
 * Returns 10-bit results (0–1023).
 */

/**
 * @brief Initialize the ADC.
 * Configures AVCC as reference voltage.
 * Sets prescaler to 128 (125kHz @ 16MHz).
 * Enables the ADC.
 */
void ADC_Init(void);

/**
 * @brief Read an analog value from the specified channel.
 * Blocking function that waits for conversion to complete.
 * 
 * @param channel ADC channel (0-7).
 * @return uint16_t 10-bit ADC value (0-1023).
 */
uint16_t ADC_Read(uint8_t channel);

#endif // ADC_H
