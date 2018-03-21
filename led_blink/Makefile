led_blink.hex: led_blink.out
	objcopy -O ihex $^ $@

led_blink.out: led_blink.c
	avr-gcc -mmcu=atmega32 -o $@ $^

flash: led_blink.hex
	sudo avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^
