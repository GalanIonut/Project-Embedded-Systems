#include "drivers/timer/timer0.h"
#include "usart.h"

void USART_Init(unsigned long fosc, unsigned int baud) {
    unsigned long ubrr = (fosc / (16UL * baud)) - 1;

    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)(ubrr);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(void* buffer, uint8_t size) {
    unsigned char* data = (unsigned char*)buffer;
    for (int i = 0; i < size; i++) {
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        UDR0 = data[i];
    }
}

int USART_Receive(void* buffer) {
    unsigned char* data = (unsigned char*)buffer;
    int i = 0;
    uint32_t last_time = Millis();

    while ((Millis() - last_time <= TIMEOUT_USART) && (i < MAX_SIZE_RECEIVE_USART)) {
        if (UCSR0A & (1 << RXC0)) {
            data[i] = UDR0;
            last_time = Millis();

            if (data[i] == '\n' || data[i] == '\r') {
                break;
            }
            i++;
        }
    }

    data[i] = '\0';
    return i;
}
