# Makefile for ATmega328P Project

# Microcontroller Settings
MCU = atmega328p
F_CPU = 16000000UL

# Programmer Settings
PROGRAMMER = arduino
PORT = /dev/cu.usbserial-2140
BAUD = 57600
# BAUD = 115200

# Board Selection (default to nano)
BOARD ?= nano

# Compiler Settings
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Directories
OBJDIR = obj
BINDIR = bin
TESTDIR = test
MOCKDIR = $(TESTDIR)/mocks
COVDIR = $(BINDIR)/coverage

# Compiler Flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=gnu99
CFLAGS += -I. -Idrivers/gpio -Idrivers/interrupt -Idrivers/timer -Idrivers/eeprom -Idrivers/adc -Ibsp -Iutils

ifeq ($(BOARD), nano)
    CFLAGS += -DBOARD_NANO
else ifeq ($(BOARD), uno)
    CFLAGS += -DBOARD_UNO
else
    $(error Invalid BOARD specified. Use 'nano' or 'uno')
endif

# Source Files
SRC = src/main.c drivers/gpio/gpio.c drivers/interrupt/external_interrupt.c drivers/timer/timer0.c drivers/timer/timer1.c drivers/timer/timer2.c drivers/pwm/pwm.c drivers/eeprom/eeprom.c drivers/adc/adc.c utils/delay.c

# Object Files
# Replace .c extension with .o and prepend OBJDIR, keeping directory structure
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))

# Target Name
TARGET = $(BINDIR)/main

# Unit Test Settings
TEST_SOURCES = $(wildcard $(TESTDIR)/test_*.c)
DRIVER_SOURCES = drivers/gpio/gpio.c drivers/pwm/pwm.c drivers/timer/timer1.c drivers/timer/timer2.c drivers/adc/adc.c drivers/eeprom/eeprom.c
MOCK_SOURCES = $(MOCKDIR)/registers.c

# Rules
all: directories $(TARGET).hex

directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)/src
	@mkdir -p $(OBJDIR)/drivers/gpio
	@mkdir -p $(OBJDIR)/drivers/interrupt
	@mkdir -p $(OBJDIR)/drivers/timer
	@mkdir -p $(OBJDIR)/drivers/eeprom
	@mkdir -p $(OBJDIR)/drivers/adc
	@mkdir -p $(OBJDIR)/utils
	@mkdir -p $(BINDIR)/test
	@mkdir -p $(COVDIR)

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

flash: $(TARGET).hex
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$<:i

clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Individual Test Targets (Legacy Support)
test_gpio: directories
	gcc -I. -Isrc -Idrivers/gpio -Idrivers/interrupt -Idrivers/timer -Idrivers/eeprom -Idrivers/adc -Ibsp -Iutils -I$(TESTDIR) -I$(MOCKDIR) -DUNIT_TEST -o $(BINDIR)/test/test_gpio test/test_gpio.c $(MOCK_SOURCES)
	@echo "Running GPIO Tests..."
	@./$(BINDIR)/test/test_gpio

test_pwm: directories
	gcc -I. -Isrc -Idrivers/gpio -Idrivers/interrupt -Idrivers/timer -Idrivers/eeprom -Idrivers/adc -Ibsp -Iutils -I$(TESTDIR) -I$(MOCKDIR) -DUNIT_TEST -o $(BINDIR)/test/test_pwm test/test_pwm.c drivers/timer/timer1.c drivers/timer/timer2.c $(MOCK_SOURCES)
	@echo "Running PWM Tests..."
	@./$(BINDIR)/test/test_pwm

test_pwm_wrapper: directories
	gcc -I. -Isrc -Idrivers/gpio -Idrivers/interrupt -Idrivers/timer -Idrivers/pwm -Idrivers/eeprom -Idrivers/adc -Ibsp -Iutils -I$(TESTDIR) -I$(MOCKDIR) -DUNIT_TEST -o $(BINDIR)/test/test_pwm_wrapper test/test_pwm_wrapper.c drivers/timer/timer1.c drivers/timer/timer2.c drivers/pwm/pwm.c $(MOCK_SOURCES)
	@echo "Running PWM Wrapper Tests..."
	@./$(BINDIR)/test/test_pwm_wrapper

# Run all tests
test: test_gpio test_pwm test_pwm_wrapper
	@echo "All tests passed!"

# Code Coverage Target
coverage: directories
	@mkdir -p $(COVDIR)
	@echo "Compiling and running all tests for coverage..."
	@for f in $(TEST_SOURCES); do \
		test_name=$$(basename $$f .c); \
		echo "Processing $$test_name..."; \
		gcc --coverage -O0 -I. -Isrc -Idrivers/gpio -Idrivers/timer -Idrivers/pwm -Idrivers/adc -Idrivers/eeprom -Ibsp -Iutils -I$(TESTDIR) -I$(MOCKDIR) -DUNIT_TEST \
			$$f $(DRIVER_SOURCES) $(MOCK_SOURCES) -o $(COVDIR)/$$test_name 2>/dev/null || \
		gcc --coverage -O0 -I. -Isrc -Idrivers/gpio -Idrivers/timer -Idrivers/pwm -Idrivers/adc -Idrivers/eeprom -Ibsp -Iutils -I$(TESTDIR) -I$(MOCKDIR) -DUNIT_TEST \
			$$f $(MOCK_SOURCES) -o $(COVDIR)/$$test_name; \
		./$(COVDIR)/$$test_name; \
	done
	@echo "Generating coverage report..."
	lcov --capture --directory . --base-directory . --output-file $(COVDIR)/coverage.info --ignore-errors unsupported,unused
	lcov --remove $(COVDIR)/coverage.info '/usr/*' 'test/*' 'utils/*' --output-file $(COVDIR)/coverage_cleaned.info --ignore-errors unused,unsupported
	genhtml $(COVDIR)/coverage_cleaned.info --output-directory $(COVDIR)/html --ignore-errors unsupported
	@echo "Report generated at $(COVDIR)/html/index.html"

coverage-html: coverage

.PHONY: all flash clean directories test coverage coverage-html test_gpio test_pwm test_pwm_wrapper
