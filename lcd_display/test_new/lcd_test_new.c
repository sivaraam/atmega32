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
#include <string.h>

void write_data (const char *const data)
{
	for (uint16_t curr_char=0; curr_char < strlen (data); curr_char++)
	{
		lcd_data (*(data+curr_char));
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

	initialize_lcd();

	write_data ("Hello world!");

	lcd_goto_line_home (2);

	write_data ("!dlrow olleH");

	// Check what happens when we try to shift the display
	lcd_command (0x14);
}
