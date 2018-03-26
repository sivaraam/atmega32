/**
 * Simple program to test if it is possible to
 * read data from a port that is configured as
 * output and to see what data is obtained.
 *
 * PORTB - output
 * PORTA - input from switches (0-3: configured as output; 4-7: configured as input)
 */

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

int main(void)
{
	DDRA = 0x0F;
	DDRB = 0xFF;
	PORTB = 0xFF;

	_delay_ms (2000);

	while (1)
	{
		PORTB = 0xFF;       // LED Off
		_delay_ms (200);
		PORTB ^= 0xFF;      // LED On
		_delay_ms (200);
		PORTB ^= 0xFF;      // LED Off
		_delay_ms (200);

		// see what data we get from the PIN register for a pins configured
		// as output and pins configured as input
		PORTB = PINA;

		// wait for some time
		_delay_ms (2000);

		PORTB = 0xFF;        // LED Off
		_delay_ms (200);
		PORTB ^= 0xFF;       // LED On
		_delay_ms (200);
		PORTB ^= 0xFF;       // LED Off
		_delay_ms (200);
		PORTB ^= 0xFF;       // LED On
		_delay_ms (200);
		PORTB ^= 0xFF;       // LED Off
		_delay_ms (200);

		// see what data we get from the PORT register
		PORTB = PORTA;

		// wait for some time
		_delay_ms (2000);
	}
}
