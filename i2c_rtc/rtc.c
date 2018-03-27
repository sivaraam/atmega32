/**
 * Simple program to read the seconds from the RTC chip in
 * the ATMEGA32 microcontroller.
 *
 * Port:
 *
 * PORTA:
 *
 * 	0 - SCL
 * 	1 - SDA
 */

#include <avr/io.h>
#include "i2c/i2c.h"

/* Debug */
#define F_CPU 1000000UL
#include <util/delay.h>

int main (void)
{
	static const uint8_t rtc_slave_addr__write = 0xD0;
	enum I2C_ack curr_ack = I2C_ACK_NACK;

	/* Initially configure the required pins as output */
	DDRA |= (1<<SCL_PIN) | (1<<SDA_PIN);

	/* Debug port */
	DDRB = 0xFF;

	while (1)
	{
		PORTB = 0xFF; // LEDs Off

		I2C_init();
		I2C_start();

		/* Write the 7-bit slave address and request for write command */
		I2C_send_byte (rtc_slave_addr__write);

		/* Check ACK */
		/* Change SDA data direction for receive */
		DDRA &= ~(1<<SDA_PIN);
		curr_ack = I2C_receive_ack();
		PORTB = ~curr_ack;
		DDRA |= (1<<SDA_PIN);

		/* Stop the communication */
		I2C_stop();

		_delay_ms (2000);
	}
}
