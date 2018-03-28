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

/**
 * RTC_init:
 *
 * Initialise the RTC (DS1307) by initialising the registers to
 * their default values.
 *
 * Returns: 0 if the initialization was successful. A non-zero value
 *          in case the initialization failed in some case.
 */
int8_t
RTC_init (void)
{
	I2C_start();

	if (I2C_send (rtc_slave_addr__write))
	{
		return 1;
	}

	if (I2C_send (seconds_addr))
	{
		return 1;
	}

	if (
		/*
		 * Seconds register (Address: 0x00):
		 *
		 * Clock Hold (CH) (7): 0 (Starts the clock)
		 * 10s digit of seconds (6-4): 0
		 * 1s digit of seconds (3-0): 0
		 */
		I2C_send (0x00) ||

		/*
		 * Minutes register (Address: 0x01):
		 *
		 * (7): 0
		 * 10s digit of minutes (6-4): 5
		 * 1s digit of minutes (3-0): 9
		 */
		I2C_send (0x59) ||

		/*
		 * Hours register (Address: 0x02)
		 *
		 * (7): 0
		 * 12/24-hour clock (6): 0 (24-hour clock)
		 * 10s digit of hours (5-4): 1
		 * 1s digit of hours (3-0): 2
		 */
		I2C_send (0x12) ||

		/*
		 * Day register (Address: 0x03)
		 *
		 * (7-3): 0
		 * Day (2-0): 03 (Wednesday; Week starts from Monday; 1-indexed)
		 */
		I2C_send (0x03) ||

		/*
		 * Date register (Address: 0x04)
		 *
		 * (7-6): 0
		 * 10s digit of date (5-4): 2
		 * 1s digit of date (3-0): 8
		 */
		I2C_send (0x28) ||

		/*
		 * Month register (Address: 0x05)
		 *
		 * (7-5): 0
		 * 10s digit of month (4): 0
		 * 1s digit of month (3-0): 3
		 */
		I2C_send (0x03) ||

		/*
		 * Year register (Address: 0x06)
		 *
		 * Note: Year is in range 00-99
		 *
		 * 10s digit of year (7-4): 1
		 * 1s digit of year (3-0): 8
		 */
		I2C_send (0x18) ||

		/*
		 * Control register (Address: 0x07):
		 *
		 * Output control (OUT) (7): 0 (logic level of output pin is 0)
		 * (6-5): 0
		 * Square wave enable (SQWE): 0 (disable square wave output)
		 * (3-2): 0
		 * Rate select (1-0) (RS1, RS0): 0 (don't cares)
		 */
		I2C_send (0x00)
	)
	{
		return 1;
	}

	return 0;
}

/**
 * RTC_read_time:
 *
 * (@hours, @minutes, @seconds): pointers used to return values
 *
 * Read the time from the RTC using I2C and return the values
 * read from the registers without any interpretation as is.
 *
 * Returns: 0 if the read was successful. Non-zero value in case
 *          of failure.
 */
int8_t
RTC_read_time (uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
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

/*
 * display_time:
 *
 * (@hours_reg, @minutes_reg, @seconds_reg): the corresponding values in the RTC registers
 *
 * Display the time in the LCD after interpreting the BCD encoded register
 * values of the time. The time is displayed in the required format (HH:MM:SS)
 * using the LCD commands as required.
 */
void
display_time (uint8_t hours_reg, uint8_t minutes_reg, uint8_t seconds_reg)
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

int
main (void)
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

		if (RTC_read_time (&hours_reg, &minutes_reg, &seconds_reg))
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
