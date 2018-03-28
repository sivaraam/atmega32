# COMPILER_OPTIONS = -std=c90
COMPILER_OPTIONS += -Wall
COMPILER_OPTIONS += -Wpedantic
COMPILER_OPTIONS += -Wextra
COMPILER_OPTIONS += -O0

rtc.hex: rtc.out
	objcopy -O ihex $^ $@

rtc.out: rtc.c i2c/i2c.c
	avr-gcc ${COMPILER_OPTIONS} -mmcu=atmega32 -o $@ $^

flash: rtc.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^
