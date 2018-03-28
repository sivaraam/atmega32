COMPILER_OPTIONS = -std=c99
COMPILER_OPTIONS += -Wall
COMPILER_OPTIONS += -Wpedantic
COMPILER_OPTIONS += -Wextra
COMPILER_OPTIONS += -O0

input_output_test_simple.hex: input_output_test_simple.out
	objcopy -O ihex $^ $@

input_output_test_simple.out: input_output_test_simple.c
	avr-gcc ${COMPILER_OPTIONS} -mmcu=atmega32 -o $@ $^

flash_test_simple: input_output_test_simple.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^


input_output_toggle.hex: input_output_toggle.out
	objcopy -O ihex $^ $@

input_output_toggle.out: input_output_toggle.c
	avr-gcc ${COMPILER_OPTIONS} -mmcu=atmega32 -o $@ $^

flash_toggle: input_output_toggle.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^


input_output_test.hex: input_output_test.out
	objcopy -O ihex $^ $@

input_output_test.out: input_output_test.c
	avr-gcc ${COMPILER_OPTIONS} -mmcu=atmega32 -o $@ $^

flash_test: input_output_test.hex
	avrdude -c avrispmkII -p m32 -P usb -U flash:w:$^
