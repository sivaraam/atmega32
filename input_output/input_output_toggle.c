/**
 * Simple program to see what happens if we toggle the data
 * direction of the pins at some time intervals and whether
 * it behaves as intended.
 *
 * PORTB - configured as output for debug
 * PORTA - configured as input and then as output
 */

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

#define DBG_PORT PORTB

int main (void)
{
	DDRB = 0xFF;

	while (1)
	{

		/*
		 * Test port as input. Input is directly assigned to output.
		 */
		DBG_PORT = 0xFF;             /* LED Off */
		_delay_ms (500);
		DBG_PORT ^= 0xFF;            /* LED On */
		_delay_ms (500);
		DBG_PORT ^= 0xFF;            /* LED Off */
		_delay_ms (500);

		DDRA = 0x00;

		/* wait for some time */
		_delay_ms (1000);

		DBG_PORT = PINA;

		/* wait for some time */
		_delay_ms (1500);

		/*
		 * Test port as output.
		 */
		DBG_PORT = 0xFF;             /* LED Off */
		_delay_ms (500);
		DBG_PORT ^= 0xFF;            /* LED On */
		_delay_ms (500);
		DBG_PORT ^= 0xFF;            /* LED Off */
		_delay_ms (500);
		DBG_PORT ^= 0xFF;            /* LED On */
		_delay_ms (500);
		DBG_PORT ^= 0xFF;            /* LED Off */
		_delay_ms (500);

		DDRA = 0xFF;
		PORTA = 0xFF;
		/* toggle the LEDs for a few times */
		unsigned count = 10;
		while (count > 0)
		{
			PORTA ^= 0xFF;
			_delay_ms (200);
			count--;
		}
	}
}
