/**
 * Simple program to see what happens if we toggle the data
 * direction of the pins at some time intervals and whether
 * it behaves as intended.
 */

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

int main (void)
{
	DDRB = 0xFF;

	while (1)
	{
		PORTB = 0xFF;             // LED Off
		_delay_ms (500);
		PORTB ^= 0xFF;            // LED On
		_delay_ms (500);
		PORTB ^= 0xFF;            // LED Off
		_delay_ms (500);

		// wait for some time
		_delay_ms (1000);

		DDRA = 0x00;
		PORTB = PINA;

		// wait for some time
		_delay_ms (1500);

		PORTB = 0xFF;             // LED Off
		_delay_ms (500);
		PORTB ^= 0xFF;            // LED On
		_delay_ms (500);
		PORTB ^= 0xFF;            // LED Off
		_delay_ms (500);
		PORTB ^= 0xFF;            // LED On
		_delay_ms (500);
		PORTB ^= 0xFF;            // LED Off
		_delay_ms (500);

		DDRA = 0xFF;
		PORTA = 0xFF;
		// toggle the LEDs for a few times
		unsigned count = 10;
		while (count > 0)
		{
			PORTA ^= 0xFF;
			_delay_ms (200);
			count--;
		}
	}
}
