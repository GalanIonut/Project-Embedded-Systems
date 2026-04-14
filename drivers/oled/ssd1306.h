#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

/*
 * @file ssd1306.h
 * @brief SSD1306 command layer — 128x64 OLED over I2C
 *
 * I2C address: 0x3C (7-bit) = 0x78 write, 0x79 read
 *
 * Display layout:
 *   8 pages × 128 columns
 *   Each page = 8 pixel rows
 *   Each byte written = 8 vertical pixels (bit0 = top)
 */

#define SSD1306_ADDR  0x3C   /* 7-bit I2C address */

/**
 * @brief Initialize and turn ON the SSD1306 display
 */
void SSD1306_Init(void);

/**
 * @brief Clear entire display (fill with 0x00)
 */
void SSD1306_Clear(void);

/**
 * @brief Set write cursor position
 *
 * @param page  Page number 0–7  (each page = 8 pixel rows)
 * @param col   Column 0–127
 */
void SSD1306_SetCursor(uint8_t page, uint8_t col);

/**
 * @brief Send one command byte to the controller
 */
void SSD1306_WriteCommand(uint8_t cmd);

/**
 * @brief Write one data byte at current cursor position (advances cursor)
 */
void SSD1306_WriteData(uint8_t data);

#endif // SSD1306_H
