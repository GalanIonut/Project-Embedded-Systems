#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <avr/io.h>

/**
 * @file gpio.h
 * @brief GPIO driver for ATmega328P (PORTB, PORTC, PORTD).
 *
 * Wraps DDR/PORT/PIN register access behind a uniform API.
 * All functions accept a port enum and a pin number (0–7).
 */

typedef enum { GPIO_PORTB, GPIO_PORTC, GPIO_PORTD } gpio_port_t;
typedef enum { GPIO_INPUT = 0, GPIO_OUTPUT = 1 }     gpio_dir_t;
typedef enum { GPIO_LOW = 0,   GPIO_HIGH = 1 }        gpio_state_t;

/**
 * @brief Configure a pin as input or output.
 *
 * @param port GPIO port (GPIO_PORTB, GPIO_PORTC, GPIO_PORTD).
 * @param pin  Pin number (0–7).
 * @param dir  Direction (GPIO_INPUT or GPIO_OUTPUT).
 */
void GPIO_Init(gpio_port_t port, uint8_t pin, gpio_dir_t dir);

/**
 * @brief Write a logic level to an output pin.
 *
 * @param port  GPIO port.
 * @param pin   Pin number (0–7).
 * @param state GPIO_HIGH or GPIO_LOW.
 */
void GPIO_Write(gpio_port_t port, uint8_t pin, gpio_state_t state);

/**
 * @brief Read the logic level of a pin.
 *
 * @param port GPIO port.
 * @param pin  Pin number (0–7).
 * @return     GPIO_HIGH or GPIO_LOW.
 */
gpio_state_t GPIO_Read(gpio_port_t port, uint8_t pin);

/**
 * @brief Toggle the current state of an output pin.
 *
 * @param port GPIO port.
 * @param pin  Pin number (0–7).
 */
void GPIO_Toggle(gpio_port_t port, uint8_t pin);

#endif // GPIO_H
