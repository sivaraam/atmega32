/**
 * Simple program to read the seconds from the RTC chip in
 * the ATMEGA32 microcontroller.
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


int8_t RTC_init (void)
{
	static const uint8_t seconds_addr = 0x00,
	                     control_addr = 0x07,
	                     seconds_config_word = 0x00,
	                     minutes_config_word = 0x59,
	                     hours_config_word = 0x12,
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
RTC_read_time (uint8_t *seconds, uint8_t *minutes, uint8_t *hours)
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

	*seconds = I2C_receive (I2C_ACK_ACK);

	*minutes = I2C_receive (I2C_ACK_ACK);

	*hours = I2C_receive (I2C_ACK_NACK);

	/* Stop the communication */
	I2C_stop();

	return 0;
}

void display_time (uint8_t hours_reg, uint8_t minutes_reg, uint8_t seconds_reg)
{
	/* Bit positions are 0-indexed */

	/* Display the hours */

	/*
	 * Bits corresponding to the tens position of the hours in
	 * the hours register: 5, 4
	 */
	lcd_data ( ((hours_reg & 0x30) >> 4) + '0');

	/*
	 * Bits corresponding to the ones position of the hours in
	 * the hours register: 3, 2, 1, 0
	 */
	lcd_data ( (hours_reg & 0x0F) + '0');


	/* Hour-Minutes separator */
	lcd_data (':');


	/* Display the minutes */

	/*
	 * Bits corresponding to the tens position of the minutes in
	 * the minutes register: 6, 5, 4
	 */
	lcd_data ( ((minutes_reg & 0x70) >> 4) + '0');

	/*
	 * Bits corresponding to the ones position of the minutes in
	 * the minutes register: 3, 2, 1, 0
	 */
	lcd_data ( (minutes_reg & 0x0F) + '0');


	/* Minutes-Seconds separator */
	lcd_data (':');


	/* Display the seconds */

	/*
	 * Bits corresponding to the tens position of the seconds in
	 * the seconds register: 6, 5, 4
	 */
	lcd_data ( (((seconds_reg & 0x70)) >> 4) + '0' );

	/*
	 * Bits corresponding to the ones position of the seconds in
	 * the seconds register: 3, 2, 1, 0
	 */
	lcd_data ( (seconds_reg & 0x0F) + '0' );
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
		/* Glow all LEDs to indicate ACK failure and exit */
		PORTB = 0x00;
		return 1;
	}

	while (1)
	{
		uint8_t seconds_reg = 0, minutes_reg = 0, hours_reg = 0;

		if (RTC_read_time (&seconds_reg, &minutes_reg, &hours_reg))
		{
			/* Glow all LEDs to indicate ACK failure and exit */
			PORTB = 0x00;
			return 1;
		}

		lcd_goto_line_home (1);

		display_time (hours_reg, minutes_reg, seconds_reg);
	}

	return 0;
}
