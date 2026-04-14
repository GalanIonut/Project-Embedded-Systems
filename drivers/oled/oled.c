#include "oled.h"
#include "ssd1306.h"
#include "font.h"
#include <avr/pgmspace.h>

/*
 * OLED TEXT DRIVER
 *
 * Each character occupies 6 pixel columns:
 *   5 from the font table + 1 empty column for spacing
 *
 * 128 columns / 6 = 21 characters per line
 */

static void write_char(uint8_t c) {
    if (c < 0x20 || c > 0x7E) c = '?';

    const uint8_t *glyph = font5x7[c - 0x20];

    for (uint8_t i = 0; i < 5; i++) {
        SSD1306_WriteData(pgm_read_byte(&glyph[i]));
    }
    SSD1306_WriteData(0x00);  /* 1 column gap between characters */
}

void OLED_Init(void) {
    SSD1306_Init();
}

void OLED_Clear(void) {
    SSD1306_Clear();
}

void OLED_ClearLine(uint8_t page) {
    SSD1306_SetCursor(page, 0);
    for (uint8_t i = 0; i < 128; i++) {
        SSD1306_WriteData(0x00);
    }
}

void OLED_Print(uint8_t page, uint8_t col, const char *str) {
    SSD1306_SetCursor(page, col * 6);
    while (*str) {
        write_char(*str++);
    }
}

void OLED_PrintNum(uint8_t page, uint8_t col, uint16_t num) {
    char buf[6];
    uint8_t i = 5;
    buf[i] = '\0';

    if (num == 0) {
        buf[--i] = '0';
    } else {
        while (num > 0 && i > 0) {
            buf[--i] = '0' + (num % 10);
            num /= 10;
        }
    }

    OLED_Print(page, col, &buf[i]);
}
