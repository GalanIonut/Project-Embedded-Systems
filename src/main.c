#include "bsp/nano.h"
#include "drivers/timer/timer0.h"
#include "drivers/servo/servo.h"
#include "utils/delay.h"

int main(void) {
    Timer0_Init();   /* system tick for Delay() */
    Servo_Init();    /* D9 / OC1A – 50 Hz, starts at 90° */

    while (1) {
        /* Sweep 0° → 180° in 1° steps */
        for (uint8_t angle = 0; angle <= 180; angle++) {
            Servo_SetAngle(angle);
            Delay(15);   /* ~15 ms between steps for smooth motion */
        }

        /* Sweep 180° → 0° in 1° steps */
        for (int16_t angle = 180; angle >= 0; angle--) {
            Servo_SetAngle((uint8_t)angle);
            Delay(15);
        }
    }

    return 0;
}
