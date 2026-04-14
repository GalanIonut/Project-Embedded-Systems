#include "bsp/nano.h"
#include "drivers/timer/timer0.h"
#include "drivers/hcsr04/hcsr04.h"
#include "drivers/servo/servo.h"
#include "drivers/buzzer/buzzer.h"
#include "drivers/gpio/gpio.h"
#include "drivers/usart/usart.h"
#include "drivers/oled/oled.h"
#include "utils/delay.h"

/*
 * TEST: HC-SR04 + Servo + Buzzer + OLED + UART
 * ═══════════════════════════════════════════════════════════════
 *
 * CABLAJ:
 * ───────────────────────────────────────────────────────────────
 *  HC-SR04:
 *    VCC   → 5V
 *    GND   → GND
 *    TRIG  → D4
 *    ECHO  → D5   ← orice pin (PCINT)
 *
 *  Servo SG90:
 *    Rosu  (VCC)    → 5V
 *    Maro  (GND)    → GND
 *    Galben (semnal) → D9   ← OC1A, fix hardware Timer1
 *
 *  Buzzer activ:
 *    +  → D12
 *    -  → GND
 *
 *  OLED SSD1306:
 *    VCC → 3.3V sau 5V
 *    GND → GND
 *    SDA → A4
 *    SCL → A5
 *
 *  LED (optional):
 *    + → D13 (via 220Ω) → GND
 *
 * NOTA Timer1:
 *   Servo_Init() trebuie apelat PRIMUL — porneste Timer1 in PWM mode.
 *   HCSR04_Init() detecteaza ca Timer1 ruleaza si nu il resetreaza.
 *   Ambele coexista: Servo pe OC1A, HC-SR04 citeste TCNT1 via PCINT.
 *
 * LOGICA:
 *   Distanta (cm)  │ Servo       │ Buzzer
 *   ───────────────┼─────────────┼──────────────────
 *   0  (eroare)    │ 90° centru  │ 3 bip-uri scurte
 *   < 10           │ 0°          │ bip rapid
 *   10 – 50        │ 0° → 180°   │ bip periodic
 *   > 50           │ 180°        │ silentios
 */

/* ── UART helpers ─────────────────────────────────────────────── */

static void uart_print(const char *s) {
    while (*s) { USART_Transmit((void *)s, 1); s++; }
}

static void uart_num(uint16_t v) {
    char buf[6];
    uint8_t i = 5;
    buf[i] = '\0';
    if (v == 0) { buf[--i] = '0'; }
    else { while (v && i) { buf[--i] = '0' + v % 10; v /= 10; } }
    uart_print(&buf[i]);
}

/* ── Map distanta → unghi servo (10–50cm → 0°–180°) ─────────── */

static uint8_t dist_to_angle(uint16_t dist) {
    if (dist < 10)  return 0;
    if (dist > 50)  return 180;
    return (uint8_t)((dist - 10) * 180 / 40);
}

/* ── Main ─────────────────────────────────────────────────────── */

int main(void) {
    Timer0_Init();
    USART_Init(16000000, 9600);

    /* Servo PRIMUL — porneste Timer1 PWM inainte de HCSR04_Init */
    Servo_Init();
    Servo_SetAngle(90);

    /* HC-SR04: Timer1 deja pornit, PCINT pe D5 */
    HCSR04_Init(0, GPIO_PORTD, 4, GPIO_PORTD, 5);

    Buzzer_Init(GPIO_PORTB, 4);
    GPIO_Init(GPIO_PORTB, 5, GPIO_OUTPUT);

    OLED_Init();
    OLED_Print(0, 0, "HC-SR04 + Servo");
    OLED_Print(1, 0, "---------------");

    uart_print("=== BOOT OK ===\r\n");

    /* Boot: 2 bip-uri scurte */
    Buzzer_Beep(100, 100);
    Buzzer_Beep(100, 0);
    Delay(500);

    while (1) {
        uint16_t dist  = HCSR04_Read(0);
        uint8_t  angle = dist_to_angle(dist);

        /* ── Servo ── */
        if (dist == 0) {
            Servo_SetAngle(90);   /* eroare → centru */
        } else {
            Servo_SetAngle(angle);
        }

        /* ── OLED ── */
        OLED_ClearLine(3);
        OLED_Print(3, 0, "Dist:");
        if (dist == 0) { OLED_Print(3, 6, "ERR"); }
        else           { OLED_PrintNum(3, 6, dist); OLED_Print(3, 10, "cm"); }

        OLED_ClearLine(5);
        OLED_Print(5, 0, "Servo:");
        if (dist == 0) { OLED_Print(5, 7, "90"); }
        else           { OLED_PrintNum(5, 7, angle); }
        OLED_Print(5, 10, "deg");

        /* ── UART ── */
        uart_print("Dist: "); uart_num(dist);
        uart_print(" cm | Servo: "); uart_num(dist == 0 ? 90 : angle);
        uart_print(" deg\r\n");

        /* ── Buzzer ── */
        GPIO_Toggle(GPIO_PORTB, 5);

        if (dist == 0) {
            Buzzer_Beep(50, 50);
            Buzzer_Beep(50, 50);
            Buzzer_Beep(50, 0);
        } else if (dist < 10) {
            Buzzer_Beep(100, 100);
        } else if (dist < 30) {
            Buzzer_Beep(50, 400);
        } else {
            Delay(300);
        }

        Delay(100);
    }

    return 0;
}
