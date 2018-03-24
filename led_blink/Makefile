led_blink.hex: led_blink.out
	objcopy -O ihex $^ $@

led_blink.out: led_blink.c
	avr-gcc -mmcu=atmega32 -o $@ $^ -O3

flash: led_blink.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^
