#include "bsp/nano.h"
#include "drivers/timer/timer0.h"
#include "drivers/hcsr04/hcsr04.h"
#include "drivers/buzzer/buzzer.h"
#include "drivers/gpio/gpio.h"
#include "drivers/usart/usart.h"
#include "drivers/oled/oled.h"
#include "utils/delay.h"

/*
 * HC-SR04 + BUZZER + UART DEBUG
 *
 * PINOUT:
 *   HC-SR04: TRIG → D4 (PD4), ECHO → D8 (PB0) FIXED ICP
 *   Buzzer:  +    → D12 (PB4), - → GND
 *   LED:     +    → D13 (PB5) via 220Ω, - → GND
 *   UART:    TX   → D1 (RX pe USB-Serial)
 *
 * macOS debug: screen /dev/cu.usbserial-* 57600
 */

/* ── UART helpers ─────────────────────────────────────────────── */

static void uart_print(const char *s) {
    while (*s) {
        USART_Transmit((void *)s, 1);
        s++;
    }
}

static void uart_print_uint(uint16_t val) {
    char buf[6];
    uint8_t i = 5;
    buf[i] = '\0';
    if (val == 0) {
        buf[--i] = '0';
    } else {
        while (val > 0 && i > 0) {
            buf[--i] = '0' + (val % 10);
            val /= 10;
        }
    }
    uart_print(&buf[i]);
}

/* ── main ─────────────────────────────────────────────────────── */

int main(void) {
    Timer0_Init();
    USART_Init(16000000, 9600);
    OLED_Init();
    HCSR04_Init(GPIO_PORTD, 4);
    Buzzer_Init(GPIO_PORTB, 4);
    GPIO_Init(GPIO_PORTB, 5, GPIO_OUTPUT);

    OLED_Print(0, 0, "HC-SR04 + Buzzer");
    OLED_Print(1, 0, "----------------");
    OLED_Print(3, 0, "Dist:     cm");
    uart_print("=== BOOT OK ===\r\n");

    Buzzer_Beep(100, 100);
    Buzzer_Beep(100, 100);
    Buzzer_Beep(100, 0);
    Delay(500);

    while (1) {
        uint16_t dist = HCSR04_Read(GPIO_PORTD, 4);

        /* OLED: update distanta si status */
        OLED_ClearLine(3);
        OLED_Print(3, 0, "Dist:");
        if (dist == 0) {
            OLED_Print(3, 6, "ERR");
        } else {
            OLED_PrintNum(3, 6, dist);
            OLED_Print(3, 10, "cm");
        }

        OLED_ClearLine(5);
        if (dist == 0) {
            OLED_Print(5, 0, "EROARE SENZOR");
        } else if (dist < 10) {
            OLED_Print(5, 0, "!! PERICOL !!");
        } else if (dist < 20) {
            OLED_Print(5, 0, "ATENTIE");
        } else if (dist < 35) {
            OLED_Print(5, 0, "OK");
        } else {
            OLED_Print(5, 0, "DEPARTE");
        }

        /* UART debug */
        uart_print("Dist: ");
        uart_print_uint(dist);
        uart_print(" cm\r\n");

        GPIO_Toggle(GPIO_PORTB, 5);

        if (dist == 0) {
            Buzzer_Beep(500, 200);
        } else if (dist < 10) {
            Buzzer_Beep(250, 100);
        } else if (dist < 20) {
            Buzzer_Beep(500, 300);
        } else if (dist < 35) {
            Buzzer_Beep(100, 0);
        } else {
            Delay(300);
        }

        Delay(200);
    }

    return 0;
}
