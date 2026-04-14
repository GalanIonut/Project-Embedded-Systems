#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/**
 * @file i2c.h
 * @brief TWI (I2C) low-level driver for ATmega328P
 *
 * Hardware pins (FIXED):
 *   SDA → A4 (PC4)
 *   SCL → A5 (PC5)
 */

/**
 * @brief Initialize TWI peripheral at 100kHz
 */
void I2C_Init(void);

/**
 * @brief Send START + slave address + R/W bit
 *
 * @param addr_rw  (7-bit address << 1) | 0 for write, | 1 for read
 *                 Example write to 0x3C: I2C_Start(0x3C << 1)
 * @return 0 on ACK, 1 on NACK/error
 */
uint8_t I2C_Start(uint8_t addr_rw);

/**
 * @brief Send STOP condition
 */
void I2C_Stop(void);

/**
 * @brief Write one byte on the bus
 *
 * @param data  Byte to send
 * @return 0 on ACK, 1 on NACK/error
 */
uint8_t I2C_Write(uint8_t data);

#endif // I2C_H
