COMPILER_OPTIONS = -std=c99
COMPILER_OPTIONS += -Wall
COMPILER_OPTIONS += -Wpedantic
COMPILER_OPTIONS += -Wextra
COMPILER_OPTIONS += -O3

lcd_test.hex: lcd_test.out
	objcopy -O ihex $^ $@

lcd_test.out: lcd_test_new.c ../lcd/lcd.c
	avr-gcc ${COMPILER_OPTIONS} -mmcu=atmega32 -o $@ $^

flash: lcd_test.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^
