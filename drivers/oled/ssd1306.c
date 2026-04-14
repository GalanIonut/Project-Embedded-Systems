#include "ssd1306.h"
#include "drivers/i2c/i2c.h"

/*
 * SSD1306 COMMAND LAYER
 *
 * I2C control byte:
 *   0x00 → Co=0, D/C=0 → command follows
 *   0x40 → Co=0, D/C=1 → data follows
 */

void SSD1306_WriteCommand(uint8_t cmd) {
    I2C_Start(SSD1306_ADDR << 1);
    I2C_Write(0x00);  /* control byte: command mode */
    I2C_Write(cmd);
    I2C_Stop();
}

void SSD1306_WriteData(uint8_t data) {
    I2C_Start(SSD1306_ADDR << 1);
    I2C_Write(0x40);  /* control byte: data mode */
    I2C_Write(data);
    I2C_Stop();
}

void SSD1306_SetCursor(uint8_t page, uint8_t col) {
    SSD1306_WriteCommand(0xB0 | (page & 0x07));      /* set page address */
    SSD1306_WriteCommand(0x00 | (col & 0x0F));       /* set col low nibble */
    SSD1306_WriteCommand(0x10 | ((col >> 4) & 0x0F)); /* set col high nibble */
}

void SSD1306_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_SetCursor(page, 0);
        /* Send 128 zero bytes — batch in one I2C transaction for speed */
        I2C_Start(SSD1306_ADDR << 1);
        I2C_Write(0x40);  /* data mode */
        for (uint8_t col = 0; col < 128; col++) {
            I2C_Write(0x00);
        }
        I2C_Stop();
    }
}

void SSD1306_Init(void) {
    I2C_Init();

    /* Full init sequence from SSD1306 datasheet */
    SSD1306_WriteCommand(0xAE);        /* display OFF */

    SSD1306_WriteCommand(0xD5);        /* set display clock */
    SSD1306_WriteCommand(0x80);        /* ratio = 1, oscillator = mid */

    SSD1306_WriteCommand(0xA8);        /* set multiplex ratio */
    SSD1306_WriteCommand(0x3F);        /* 64 rows - 1 = 63 */

    SSD1306_WriteCommand(0xD3);        /* set display offset */
    SSD1306_WriteCommand(0x00);        /* no offset */

    SSD1306_WriteCommand(0x40);        /* set start line = 0 */

    SSD1306_WriteCommand(0x8D);        /* charge pump */
    SSD1306_WriteCommand(0x14);        /* enable internal charge pump */

    SSD1306_WriteCommand(0x20);        /* memory addressing mode */
    SSD1306_WriteCommand(0x02);        /* page addressing mode */

    SSD1306_WriteCommand(0xA1);        /* segment remap: col127 = SEG0 */
    SSD1306_WriteCommand(0xC8);        /* COM scan: remapped (top→bottom) */

    SSD1306_WriteCommand(0xDA);        /* COM pins hardware config */
    SSD1306_WriteCommand(0x12);        /* alternative COM, no remap */

    SSD1306_WriteCommand(0x81);        /* set contrast */
    SSD1306_WriteCommand(0xCF);        /* 0xCF = high contrast */

    SSD1306_WriteCommand(0xD9);        /* precharge period */
    SSD1306_WriteCommand(0xF1);

    SSD1306_WriteCommand(0xDB);        /* VCOMH deselect level */
    SSD1306_WriteCommand(0x40);

    SSD1306_WriteCommand(0xA4);        /* output follows RAM content */
    SSD1306_WriteCommand(0xA6);        /* normal display (not inverted) */

    SSD1306_Clear();

    SSD1306_WriteCommand(0xAF);        /* display ON */
}
