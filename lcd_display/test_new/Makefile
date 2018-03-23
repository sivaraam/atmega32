COMPILER_OPTIONS = -std=c99

lcd_test.hex: lcd_test.out
	objcopy -O ihex $^ $@

lcd_test.out: lcd_test_new.c
	avr-gcc ${COMPILER_OPTIONS} -mmcu=atmega32 -o $@ $^ -O3

flash: lcd_test.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^
