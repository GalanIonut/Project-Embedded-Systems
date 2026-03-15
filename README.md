# ATmega328P Low-Level Driver Development
This repository contains an embedded systems project focused on developing custom, bare-metal drivers for the ATmega328P microcontroller. Instead of using high-level libraries (like the standard Arduino IDE framework), this project interacts directly with the hardware registers to maximize efficiency and gain a deeper understanding of the AVR architecture.
# 🚀 Project Overview
The goal of this project is to implement a modular driver suite for various peripherals of the ATmega328P. By bypassing abstraction layers, we ensure minimal code overhead and precise control over the MCU's timing and resources.

Note: This project is part of the Embedded Systems curriculum. It emphasizes low-level C programming and datasheet-driven development.
----------

# 🛠 Hardware & Tools
Microcontroller: ATmega328P (8-bit AVR)

* Clock Speed: 16 MHz (External Crystal)

* Compiler: avr-gcc

* Uploader: avrdude

* Communication: UART (via USB-to-Serial converter)
----------

# 📂 Project Structure
Currently, the project focuses on core system initialization. Future updates will include:

* [ ] GPIO Driver: Individual pin configuration and port manipulation.

* [ ] UART Driver: Serial communication with interrupt support.

* [ ] Timer/Counter: PWM generation and precise delay functions.

* [ ] ADC: Analog-to-digital conversion for sensor integration.

* [ ] I2C/SPI: Communication protocols for external peripherals.
----------
# 📜 License
This project is developed for educational purposes. Feel free to use the code as a reference for your own bare-metal adventures.
