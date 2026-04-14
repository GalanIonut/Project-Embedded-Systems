#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

/**
 * @brief Blocking delay function using software loop and NOPs.
 *
 * @param ms Number of milliseconds to delay.
 */
void Delay(uint32_t ms);

/**
 * @brief Fixed 10 microsecond delay using software loop.
 */
void Delay10us(void);

#endif // DELAY_H
