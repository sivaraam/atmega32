/**
 * Blinks the LED by configuring all Port B pins as output and 
 * toggling their state at some delay.
 *
 * Input: None
 * Output: All pins of Port B
 */

#include <avr/io.h>

void count_down_to_zero (unsigned from)
{
	while (from != 0)
	{
		from--;
	}
}

int main (void)
{
	// configure PORT B for output
	DDRB = 0xff;

	// initially write logic high to port B
	PORTB = 0x00;

	while (1)
	{
		PORTB = 0xff;
		count_down_to_zero (10000u);
		PORTB = 0x00;
		count_down_to_zero (10000u);
	}
}
