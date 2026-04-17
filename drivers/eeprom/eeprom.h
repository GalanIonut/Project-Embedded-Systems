#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

/**
 * @file eeprom.h
 * @brief Internal EEPROM read/write driver for ATmega328P.
 *
 * ATmega328P has 1 KB of EEPROM (addresses 0–1023), rated for ~100 000 write cycles.
 * EEPROM_Update() skips the write if the stored value already matches, extending lifetime.
 */

/**
 * @brief Read a byte from the specified EEPROM address.
 * 
 * @param address The 0-1023 address to read from.
 * @return uint8_t The data byte read.
 */
uint8_t EEPROM_Read(uint16_t address);

/**
 * @brief Write a byte to the specified EEPROM address.
 * This function waits for previous writes to complete.
 * 
 * @param address The 0-1023 address to write to.
 * @param data The data byte to write.
 */
void EEPROM_Write(uint16_t address, uint8_t data);

/**
 * @brief Update a byte at the specified EEPROM address.
 * Only writes if the stored value is different from the new value.
 * This helps prolong EEPROM life.
 * 
 * @param address The 0-1023 address to update.
 * @param data The data byte to write.
 */
void EEPROM_Update(uint16_t address, uint8_t data);

#endif // EEPROM_H
