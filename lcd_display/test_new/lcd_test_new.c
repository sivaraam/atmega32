/**
 * Program to test the how to interface with an LCD.
 *
 * Port C - data pins to LCD
 * Port D:
 *
 * 	Pin 0: Enable pin of LCD
 * 	Pin 1: Read/Write pin of LCD
 * 	Pin 2: RS pin of LCD
 *
 */

#include "../lcd/lcd.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void write_data (const char *const data)
{
	for (uint16_t curr_char=0; curr_char < strlen (data); curr_char++)
	{
		while ((PINC & 0x01) == 0x01);

		lcd_data (*(data+curr_char));

		/*
		 * Configure PORTD as input to receive the BF and
		 * address counter status
		 */
		DDRD = 0x00;

		lcd_read_bf ();
		PORTB = PIND;

		/**
		 * Re-configure PORTD as output
		 */
		DDRD = 0xFF;

		/*
		 * Delay to avoid continuous data inputs.
		 */
		_delay_ms (1000);
	}
}

int main(void)
{
	/**
	 * Initialize ports used for LCD as outputs.
	 */
	DDRD = 0xFF;

	/**
	 * Note that only three pins of PORT A are actually
	 * used to connect to the LCD but all ports are initialized
	 * to simplify changing the value of the three pins.
	 */
	DDRA = 0xFF;

	/* Initialize all pins of PORTB as output for testing purposes. */
	DDRB = 0xFF;

	initialize_lcd();

	write_data ("Hello world!");

	lcd_goto_line_home (2);

	write_data ("!dlrow olleH");

	// Check what happens when we try to shift the display
	lcd_command (0x14);
}
