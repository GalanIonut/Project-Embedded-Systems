#ifndef OLED_H
#define OLED_H

#include <stdint.h>

/**
 * @file oled.h
 * @brief High-level OLED text driver (128x64 SSD1306)
 *
 * 8 pages × 128 columns. Each character = 6 pixels wide (5 font + 1 space).
 * → 21 characters per line, 8 lines total.
 *
 * Usage:
 *   OLED_Init();
 *   OLED_Clear();
 *   OLED_Print(0, 0, "Dist: ");
 *   OLED_PrintNum(0, 6, 123);
 */

/**
 * @brief Initialize display (calls SSD1306_Init internally)
 */
void OLED_Init(void);

/**
 * @brief Clear all pixels
 */
void OLED_Clear(void);

/**
 * @brief Print a string at given page/column
 *
 * @param page  0–7
 * @param col   Character column 0–20 (will be multiplied by 6 internally)
 * @param str   Null-terminated ASCII string
 */
void OLED_Print(uint8_t page, uint8_t col, const char *str);

/**
 * @brief Print an unsigned number at given page/column
 *
 * @param page  0–7
 * @param col   Character column 0–20
 * @param num   Value to display
 */
void OLED_PrintNum(uint8_t page, uint8_t col, uint16_t num);

/**
 * @brief Clear a single line (page) by filling with spaces
 *
 * @param page  0–7
 */
void OLED_ClearLine(uint8_t page);

#endif // OLED_H
