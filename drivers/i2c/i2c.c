#include "i2c.h"
#include <avr/io.h>
#include <util/twi.h>

/*
 * TWI (I2C) DRIVER — ATmega328P
 *
 * SCL frequency = F_CPU / (16 + 2 * TWBR * prescaler)
 * 100kHz @ 16MHz, prescaler=1: TWBR = (16000000/100000 - 16) / 2 = 72
 */

#define I2C_FREQ  100000UL
#define TWBR_VAL  ((F_CPU / I2C_FREQ - 16) / 2)

void I2C_Init(void) {
    TWSR = 0x00;         /* prescaler = 1 */
    TWBR = TWBR_VAL;
    TWCR = (1 << TWEN);  /* enable TWI */
}

uint8_t I2C_Start(uint8_t addr_rw) {
    /* START condition */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    /* Send address + R/W */
    TWDR = addr_rw;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    return (TW_STATUS == TW_MT_SLA_ACK) ? 0 : 1;
}

void I2C_Stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO));
}

uint8_t I2C_Write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return (TW_STATUS == TW_MT_DATA_ACK) ? 0 : 1;
}
