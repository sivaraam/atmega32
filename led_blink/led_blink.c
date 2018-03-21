/**
 * Blinks the LED by configuring all Port B pins as output and 
 * toggling their state at some delay.
 *
 * Input: None
 * Output: All pins of Port B
 */

#include <avr/io.h>

#define F_CPU 1000000UL

#include <util/delay.h>

int main (void)
{
	// configure PORT B for output
	DDRB = 0xff;

	// initially write logic high to port B
	PORTB = 0x00;

	while (1)
	{
		PORTB ^= 0xff;
		_delay_ms (1000u);
	}
}
