# Ball Balancer — ATmega328P

A bare-metal embedded system that balances a ball on a flat surface using a **PID control loop**, an **SG90 servo motor**, and **two HC-SR04P ultrasonic sensors** for position detection — all running on an ATmega328P (Arduino Nano / Uno) at 16 MHz, written entirely in C with no Arduino libraries.

---

## Table of Contents

- [Overview](#overview)
- [How It Works](#how-it-works)
- [Hardware Requirements](#hardware-requirements)
- [Pin Mapping](#pin-mapping)
- [Project Structure](#project-structure)
- [Driver Reference](#driver-reference)
- [PID Tuning](#pid-tuning)
- [Build & Flash](#build--flash)
- [Running Tests](#running-tests)
- [Contributors](#contributors)

---

## Overview

The system tilts a platform with an SG90 servo to keep a ball at a target position.
Two **HC-SR04P** ultrasonic sensors measure the ball's distance from each end of the platform.
A PID controller computes the tilt correction and drives the servo in real time.
All three PID gains (Kp, Ki, Kd) are tunable live via potentiometers without re-flashing.

```
  HC-SR04P #1          HC-SR04P #2
  (left end)           (right end)
      │                     │
      └─────────┬───────────┘
                │  ball position (distance delta)
                ▼
        ┌──────────────┐    error     ┌─────────────┐   servo angle   ┌───────────┐
        │   Setpoint   │ ──────────► │     PID     │ ──────────────► │ SG90 PWM  │
        │   (center)   │             │  Controller │                  │  (Timer1) │
        └──────────────┘             └─────────────┘                  └───────────┘
                                            ▲
                                Kp (A0) ────┤
                                Ki (A1) ────┤  (live potentiometer tuning)
                                Kd (A2) ────┘
```

---

## How It Works

### Position Sensing — HC-SR04P (3–5.5 V)

Two HC-SR04P sensors are mounted at opposite ends of the platform:

- **Sensor 1** measures distance from the left end to the ball → `d1`
- **Sensor 2** measures distance from the right end to the ball → `d2`

Ball position is derived as:

```
position = d1 - d2      (negative = ball left of center, positive = ball right)
setpoint = 0            (center of platform)
error    = setpoint - position = -(d1 - d2)
```

Each sensor fires a 10 µs trigger pulse; the firmware measures the echo pulse width and converts it to distance:

```
distance_cm = echo_pulse_µs / 58
```

### Control Loop

1. **Trigger** both HC-SR04P sensors and read echo durations.
2. **Compute** `error = setpoint − (d1 − d2)`.
3. **Run PID** — `calculate_command(Kp, Ki, Kd, error, prev_error)`:

   ```
   u(t) = Kp·e(t) + Ki·∫e dt + Kd·(Δe / Δt)
   ```

4. **Map** PID output → servo angle (0°–180°) and update via `Servo_SetAngle()`.
5. **Repeat** every 300 ms (`TIME_STEP = 0.3f`).

---

## Hardware Requirements

| Component | Part | Details |
|-----------|------|---------|
| MCU | ATmega328P | Arduino Nano or Uno, 16 MHz |
| Servo | SG90 | Signal → D9, 5 V, GND |
| Ultrasonic sensor × 2 | HC-SR04P | 3–5.5 V, one each end of platform |
| Potentiometer × 3 | 10 kΩ | Kp (A0), Ki (A1), Kd (A2) |
| Power supply | — | 5 V, ≥ 1 A (servo draws up to 700 mA under load) |
| Programmer | — | USB–Serial (Arduino bootloader) or ISP |

> **Power note:** Power the servo from an external 5 V rail, not directly from the Arduino's 5 V pin — share a common GND.
> The HC-SR04P works at 3–5.5 V and can be powered directly from the Arduino 5 V pin.

---

## Pin Mapping

### Servo

| Arduino Pin | AVR Pin | Function |
|-------------|---------|----------|
| D9 | PB1 / OC1A | SG90 PWM signal (50 Hz, Timer1) |

### HC-SR04P Sensors

| Arduino Pin | AVR Pin | Sensor | Signal |
|-------------|---------|--------|--------|
| D4 | PD4 | Sensor 1 — left end | TRIG |
| D5 | PD5 | Sensor 1 — left end | ECHO |
| D6 | PD6 | Sensor 2 — right end | TRIG |
| D7 | PD7 | Sensor 2 — right end | ECHO |

### PID Gain Potentiometers

| Arduino Pin | ADC Channel | PID Gain | Range |
|-------------|-------------|----------|-------|
| A0 | ADC0 | Kp | 0.0 – 5.0 |
| A1 | ADC1 | Ki | 0.0 – 2.0 |
| A2 | ADC2 | Kd | 0.0 – 0.5 |

### Debug

| Arduino Pin | Function |
|-------------|----------|
| D0 / D1 | USART RX / TX — serial debug @ 9600 baud |
| D13 | Onboard LED — control loop heartbeat |

---

## Project Structure

```
Project-Embedded-Systems/
├── src/
│   └── main.c                  # Application entry point & control loop
├── drivers/
│   ├── adc/                    # 10-bit ADC driver (AVCC reference, prescaler 128)
│   ├── eeprom/                 # EEPROM read/write driver
│   ├── gpio/                   # GPIO input/output/toggle driver
│   ├── interrupt/              # External interrupt driver (INT0, INT1)
│   ├── pid/                    # PID controller (ADC-tunable gains via pots)
│   ├── pwm/                    # Generic PWM driver (Timer1 + Timer2)
│   │                           #   PWM_SetDutyCycle accepts pulse width in µs
│   ├── servo/                  # SG90 servo driver (built on PWM driver)
│   ├── timer/
│   │   ├── timer0.c/h          # System tick — 1 ms CTC ISR → Millis()
│   │   ├── timer1.c/h          # 16-bit Fast PWM (Mode 14, ICR1 as TOP)
│   │   └── timer2.c/h          # 8-bit Fast PWM (Mode 3, TOP = 0xFF)
│   └── usart/                  # USART driver — serial transmit/receive
├── bsp/
│   ├── bsp.h                   # Board selector (BOARD_NANO / BOARD_UNO)
│   ├── nano.h                  # Arduino Nano pin definitions
│   └── uno.h                   # Arduino Uno pin definitions
├── utils/
│   └── delay.c/h               # Millisecond delay (Millis-based, Timer0)
├── test/                       # Unit tests (host-compiled with register mocks)
│   ├── mocks/
│   │   └── registers.c         # AVR register mock for host testing
│   ├── test_gpio.c
│   ├── test_pwm.c
│   └── test_pwm_wrapper.c
└── Makefile
```

---

## Driver Reference

### Servo (`drivers/servo/`)

```c
Servo_Init();              // Init Timer1 at 50 Hz on D9, park at 90°
Servo_SetAngle(uint8_t);   // 0–180°, clamped — ~4 000 positions (16-bit Timer1)
Servo_Stop();              // Disable PWM output
```

**Pulse mapping:**

| Angle | Pulse width | OCR1A ticks (prescaler = 8) |
|-------|-------------|------------------------------|
| 0°    | 500 µs      | 1 000                        |
| 90°   | 1 500 µs    | 3 000                        |
| 180°  | 2 500 µs    | 5 000                        |

---

### PWM (`drivers/pwm/`)

```c
PWM_Init(port, pin, freq_hz);           // Configure timer for desired frequency
PWM_SetDutyCycle(port, pin, pulse_us);  // Set pulse width in microseconds
PWM_Stop(port, pin);                    // Disable output
```

The prescaler is read from the hardware registers (`TCCR1B` / `TCCR2B`) at runtime, so `PWM_SetDutyCycle` stays accurate for any frequency set at init.

> Timer1 (D9, D10) supports exact 50 Hz — recommended for servo.
> Timer2 (D3, D11) is 8-bit; closest achievable to 50 Hz is ~61 Hz.

---

### PID (`drivers/pid/`)

```c
float kp  = calculate_Kp();                               // A0 pot → [0.0, 5.0]
float ki  = calculate_Ki();                               // A1 pot → [0.0, 2.0]
float kd  = calculate_Kd();                               // A2 pot → [0.0, 0.5]
float cmd = calculate_command(kp, ki, kd, error, prev);   // compute u(t)
```

`TIME_STEP` is defined as `0.3f` (300 ms) in `pid.h` — update this constant if you change the control loop period.

---

### ADC (`drivers/adc/`)

```c
ADC_Init();                   // AVCC reference, prescaler 128 (125 kHz @ 16 MHz)
uint16_t v = ADC_Read(ch);    // Blocking 10-bit read, returns 0–1023
```

---

### Timer0 / Millis (`drivers/timer/timer0`)

```c
Timer0_Init();                // CTC mode, 1 ms tick, enables global interrupts
uint32_t t = Millis();        // Milliseconds since startup (atomic 32-bit read)
```

---

## PID Tuning

Start with all gains at zero and increase one at a time:

| Step | Gain | Action | Expected result |
|------|------|--------|-----------------|
| 1 | **Kp** | Increase slowly | Ball starts moving toward center |
| 2 | **Kp** | Keep increasing | Ball oscillates around center |
| 3 | **Kd** | Increase slowly | Oscillation dampens |
| 4 | **Ki** | Add a small value | Removes steady-state offset |

**Troubleshooting:**

| Symptom | Fix |
|---------|-----|
| Ball overshoots heavily | Reduce **Kp** or increase **Kd** |
| Ball is slow to reach setpoint | Increase **Kp** |
| Ball settles slightly off center | Increase **Ki** |
| High-frequency oscillations | Reduce **Kd** |
| Sensor readings noisy / jittery | Average multiple HC-SR04P readings per cycle |

Gains are sampled at the start of every control cycle so changes take effect immediately without re-flashing.

---

## Build & Flash

**Prerequisites:** `avr-gcc`, `avr-libc`, `avrdude`

```bash
# Build for Arduino Nano (default)
make

# Build for Arduino Uno
make BOARD=uno

# Flash via USB — default port is COM6, adjust as needed
make flash

# Flash on Linux / macOS
make flash PORT=/dev/ttyUSB0

# Clean build artefacts
make clean
```

---

## Running Tests

Tests compile and run on the host machine (no AVR hardware needed) using register mocks.

```bash
# Run all tests
make test

# Run individual test suites
make test_gpio
make test_pwm
make test_pwm_wrapper

# Generate HTML coverage report (requires lcov + genhtml)
make coverage
# open bin/coverage/html/index.html
```

---

## Contributors

| Name | Role |
|------|------|
| **Galan Ionut Catalin** | Firmware architecture, PWM & Servo driver, PID integration |
| **Popescu Emanuel Stefan** | PID controller, ADC tuning system, hardware design |
