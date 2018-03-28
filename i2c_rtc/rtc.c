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

#include "../lcd_display/lcd/lcd.h"

static const uint8_t rtc_slave_addr__write = 0xD0,
                     rtc_slave_addr__read  = 0xD1;

static const uint8_t seconds_addr = 0x00;

int8_t I2C_send (uint8_t byte)
{
	enum I2C_ack curr_ack = I2C_ACK_ACK;

	I2C_send_byte (byte);

	/* Check ACK */
	curr_ack = I2C_receive_ack();

	if (curr_ack == I2C_ACK_NACK)
	{
		return 1;
	}

	return 0;
}

int8_t RTC_init (void)
{
	static const uint8_t seconds_addr = 0x00,
	                     control_addr = 0x07,
	                     seconds_config_word = 0x00,
	                     minutes_config_word = 0x00,
	                     hours_config_word = 0x00,
	                     control_config_word = 0x00;

	I2C_start();

	/* Write the 7-bit slave address and request for write command */
	if (I2C_send (rtc_slave_addr__write))
	{
		return 1;
	}

	/* Start configuring the registers */
	if (I2C_send (control_addr))
	{
		return 1;
	}

	if (I2C_send (control_config_word))
	{
		return 1;
	}

	/* Re-start to configure remaining registers */
	I2C_start();

	if (I2C_send (rtc_slave_addr__write))
	{
		return 1;
	}

	if (I2C_send (seconds_addr))
	{
		return 1;
	}

	/* start the RTC and configure the initial seconds value */
	if (I2C_send (seconds_config_word))
	{
		return 1;
	}

	/* configure the initial minutes value */
	if (I2C_send (minutes_config_word))
	{
		return 1;
	}

	/* configure the type of clock (24Hours) and the initial hours value */
	if (I2C_send (hours_config_word))
	{
		return 1;
	}

	return 0;
}

int8_t
RTC_read_seconds (uint8_t *seconds)
{
	/* Re-start communication to read the value */
	I2C_start();

	/* Request write to specify the address of the seconds register */
	if (I2C_send (rtc_slave_addr__write))
	{
		return 1;
	}

	/* Send seconds register address */
	if (I2C_send (seconds_addr))
	{
		return 1;
	}

	/* Re-start to read the value from the register */
	I2C_start ();

	/* Request to read value from the register address */
	if (I2C_send (rtc_slave_addr__read))
	{
		return 1;
	}

	*seconds = I2C_receive_byte ();

	/* Send NACK */
	I2C_send_ack (I2C_ACK_NACK);

	/* Stop the communication */
	I2C_stop();

	return 0;
}

/**
 * get_seconds:
 *
 * @seconds_reg: the bits in the seconds register
 * @tens_ASCII: used to return the ASCII value of the tens position
 * @ones_ASCII: used to return the ASCII value of the ones position
 *
 * Given the value of the bits in the seconds register of the RTC
 * return the ASCII value of the 10s and 1s digit.
 */
void get_seconds_ascii (uint8_t seconds_reg, uint8_t *tens_ASCII, uint8_t *ones_ASCII)
{
	/*
	 * The three bits corresponding to the tens position
	 * of the seconds are 6, 5, 4 (0-indexed).
	 */
	*tens_ASCII = (((seconds_reg & 0x70)) >> 4) + '0';

	/*
	 * The lower 4-bits correspond to the ones position
	 * of the seconds.
	 */
	*ones_ASCII = (seconds_reg & 0x0F) + '0';
}

int main (void)
{
	/* Debug port */
	DDRB = 0xFF;
	PORTB = 0xFF;

	/* Initialise the LCD */
	DDRD = 0xFF;
	DDRA |= 0x07;
	initialize_lcd ();

	/* Initialize the port pins used by I2C */
	I2C_init();

	/* Initialise the RTC */
	if (RTC_init())
	{
		PORTB = 0x00; // glow all LEDs of PORTB and break out of the loop
		return 1;
	}

	while (1)
	{
		uint8_t seconds_reg = 0, seconds_tens_ASCII = 0, seconds_ones_ASCII = 0;

		PORTB = 0xFF; // LEDs Off

		if (RTC_read_seconds (&seconds_reg))
		{
			PORTB = 0x00;
			return 1;
		}

		get_seconds_ascii (seconds_reg, &seconds_tens_ASCII, &seconds_ones_ASCII);

		lcd_goto_line_home (1);
		lcd_data (seconds_tens_ASCII);
		lcd_data (seconds_ones_ASCII);
		_delay_ms (400);  /* wait for some time before updating the value */
	}

	return 0;
}
