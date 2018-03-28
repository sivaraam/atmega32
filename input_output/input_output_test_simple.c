/**
 * Simple program to see the initial values of the
 * port pins in different scenarios.
 *
 * PORTA - input
 * PORTB - output
 * PORTD - output (values)
 */

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

int main (void)
{
	/* configure the ports */
	DDRA = 0x00;
	DDRB = 0xFF;
	DDRD = 0xFF;

	/* blink LED once for input port test */
	PORTB = 0x00; /* LED On */
	_delay_ms (500);
	PORTB = 0xFF; /*LED Off */
	_delay_ms (500);

	PORTB = PINA;
	_delay_ms (1000);

	/* Clear LED */
	PORTB = 0xFF;

	/* blink LED twice for input port (pull-up resistor off) test */
	PORTB = 0x00; /* LED On */
	_delay_ms (500);
	PORTB = 0xFF; /*LED Off */
	_delay_ms (500);
	PORTB = 0x00; /* LED On */
	_delay_ms (500);
	PORTB = 0xFF; /*LED Off */
	_delay_ms (500);

	PORTA = 0x00;
	PORTB = PINA;
	_delay_ms (1000);

	/* Clear LED */
	PORTB = 0xFF;

	/* blink LED twice for input port (pull-up resistor off) test */
	PORTB = 0x00; /* LED On */
	_delay_ms (500);
	PORTB = 0xFF; /*LED Off */
	_delay_ms (500);
	PORTB = 0x00; /* LED On */
	_delay_ms (500);
	PORTB = 0xFF; /*LED Off */
	_delay_ms (500);
	PORTB = 0x00; /* LED On */
	_delay_ms (500);
	PORTB = 0xFF; /*LED Off */
	_delay_ms (500);


	PORTB = PORTD;
	_delay_ms (1000);

	return 0;
}
