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

static const uint8_t seconds_register_addr = 0x00;

/**
 * Structures used to easily access different bit sequences in the
 * BCD encoded RTC register values.
 *
 * Bit positions in comments are 0-indexed.
 */

/**
 * RTC_time:
 *
 * Structure used to hold the values of the time related registers.
 */
struct RTC_time
{
	union RTC_time_hours
	{
		uint8_t register_val;
		struct RTC_time_hours_positions
		{

			/*
			 * Bits corresponding to the ones position of the
			 * hours in the hours register: 3, 2, 1, 0
			 */
			unsigned ones_pos : 4;

			/*
			 * Bits corresponding to the tens position of the
			 * hours in the hours register: 5, 4
			 */
			unsigned tens_pos : 2;

		} positions;
	} hours;

	union RTC_time_minutes_seconds
	{
		uint8_t register_val;
		struct RTC_time_minutes_seconds_positions
		{

			/*
			 * Bits corresponding to the ones position of the
			 * minutes (seconds) in the minutes (seconds) register: 3, 2, 1, 0
			 */
			unsigned ones_pos : 4;

			/*
			 * Bits corresponding to the tens position of the
			 * minutes (seconds) in the minutes (seconds) register: 6, 5, 4
			 */
			unsigned tens_pos : 3;

		} positions;
	} minutes, seconds;
};

/*
 * RTC_date:
 *
 * Structure used to hold the values of the date related registers.
 */
struct RTC_date
{
	union RTC_date_date
	{
		uint8_t register_val;
		struct RTC_date_date_positions
		{

			/*
			 * Bits corresponding to the ones position of the
			 * date in the date register: 3, 2, 1, 0
			 */
			unsigned ones_pos : 4;

			/*
			 * Bits corresponding to the tens position of the
			 * date in the date register: 5, 4
			 */
			unsigned tens_pos : 2;

		} positions;
	} date;

	union RTC_date_month
	{
		uint8_t register_val;
		struct RTC_date_month_positions
		{

			/*
			 * Bits corresponding to the ones position of the
			 * month in the month register: 3, 2, 1, 0
			 */
			unsigned ones_pos : 4;

			/*
			 * Bits corresponding to the tens position of the
			 * month in the month register: 4
			 */
			unsigned tens_pos : 1;

		} positions;
	} month;

	union RTC_date_year
	{
		uint8_t register_val;
		struct RTC_date_year_positions
		{

			/*
			 * Bits corresponding to the ones position of the
			 * year in the year register: 3, 2, 1, 0
			 */
			unsigned ones_pos : 4;

			/*
			 * Bits corresponding to the tens position of the
			 * year in the year register: 7, 6, 5, 4
			 */
			unsigned tens_pos : 4;

		} positions;
	} year;

	union RTC_date_dow
	{
		uint8_t register_val;
		struct RTC_date_dow_positions
		{

			/*
			 * Bits corresponding to the ones position of the
			 * day of the week (DOW) in the day of the week register: 2, 1, 0
			 */
			unsigned ones_pos : 4;

		} positions;
	} dow;
};

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

	if (I2C_send (seconds_register_addr))
	{
		return 1;
	}

	if (
		/*
		 * Seconds register (Address: 0x00):
		 *
		 * Clock Hold (CH) (7): 0 (Starts the clock)
		 * 10s digit of seconds (6-4): 5
		 * 1s digit of seconds (3-0): 0
		 */
		I2C_send (0x50) ||

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
		 * 10s digit of hours (5-4): 2
		 * 1s digit of hours (3-0): 3
		 */
		I2C_send (0x23) ||

		/*
		 * Day register (Address: 0x03)
		 *
		 * (7-3): 0
		 * Day (2-0): 01 (Wednesday; Week starts from Monday; 1-indexed)
		 */
		I2C_send (0x01) ||

		/*
		 * Date register (Address: 0x04)
		 *
		 * (7-6): 0
		 * 10s digit of date (5-4): 3
		 * 1s digit of date (3-0): 1
		 */
		I2C_send (0x31) ||

		/*
		 * Month register (Address: 0x05)
		 *
		 * (7-5): 0
		 * 10s digit of month (4): 1
		 * 1s digit of month (3-0): 2
		 */
		I2C_send (0x12) ||

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
 * (@time): pointer to the structure used to return the values
 *          of the registers related to time
 *
 * Read the time from the RTC using I2C and return the values
 * read from the registers without any interpretation as is.
 *
 * Returns: 0 if the read was successful. Non-zero value in case
 *          of failure.
 */
int8_t
RTC_read_time (struct RTC_time *time)
{
	/* Start communication to read the value */
	I2C_start();

	/* Request write to specify the address of the seconds register */
	if (I2C_send (rtc_slave_addr__write))
	{
		return 1;
	}

	/* Send seconds register address */
	if (I2C_send (seconds_register_addr))
	{
		return 1;
	}

	/* Re-start to read the value from the registers */
	I2C_start ();

	/* Request to read value from the registers */
	if (I2C_send (rtc_slave_addr__read))
	{
		return 1;
	}

	time->seconds.register_val = I2C_receive (I2C_ACK_ACK);

	time->minutes.register_val = I2C_receive (I2C_ACK_ACK);

	time->hours.register_val = I2C_receive (I2C_ACK_NACK);

	/* Stop the communication */
	I2C_stop();

	return 0;
}

/**
 * RTC_read_date:
 *
 * (@date): pointer to the structure used to return the values
 *          of the registers related to date
 *
 * Read the date from the RTC using I2C and return the values
 * read from the registers without any interpretation as is.
 *
 * Returns: 0 if the read was successful. Non-zero value in case
 *          of failure.
 */
int8_t
RTC_read_date (struct RTC_date *date)
{
	static const uint8_t day_register_addr = 0x03;

	/* Start communication to read the value */
	I2C_start();

	/* Request write to specify the address of the day register */
	if (I2C_send (rtc_slave_addr__write))
	{
		return 1;
	}

	/* Send day register address */
	if (I2C_send (day_register_addr))
	{
		return 1;
	}

	/* Re-start to read the value from the registers */
	I2C_start ();

	/* Request to read value from the registers */
	if (I2C_send (rtc_slave_addr__read))
	{
		return 1;
	}

	date->dow.register_val = I2C_receive (I2C_ACK_ACK);

	date->date.register_val = I2C_receive (I2C_ACK_ACK);

	date->month.register_val = I2C_receive (I2C_ACK_ACK);

	date->year.register_val = I2C_receive (I2C_ACK_NACK);

	/* Stop the communication */
	I2C_stop();

	return 0;

}

/**
 * Functions used to display the time and date in the required format[1].
 * The functions use the required LCD commands as and when required.
 *
 * [1]: Required format:
 *
 * 		Time: HH:MM:SS
 * 		Date: DD/MM/YY DOW
 */

/**
 * display_time:
 *
 * (@time): the structure containing the values in the RTC registers
 *
 * Display the time in the LCD in the required format (see above).
 */
void
display_time (struct RTC_time time)
{
	/* Display the hours */
	lcd_data (time.hours.positions.tens_pos + '0');
	lcd_data (time.hours.positions.ones_pos + '0');

	/* Hour-Minutes separator */
	lcd_data (':');

	/* Display the minutes */
	lcd_data (time.minutes.positions.tens_pos + '0');
	lcd_data (time.minutes.positions.ones_pos + '0');

	/* Minutes-Seconds separator */
	lcd_data (':');

	/* Display the seconds */
	lcd_data (time.seconds.positions.tens_pos + '0' );
	lcd_data (time.seconds.positions.ones_pos + '0' );
}

/**
 * display_date:
 *
 * (@date): the structure containing the values in the RTC registers
 *
 * Display the date in the LCD in the required format (see above).
 */
void
display_date (struct RTC_date date)
{
	static const char dow_strings[8][3] = {
		"",
		"MON",
		"TUE",
		"WED",
		"THU",
		"FRI",
		"SAT",
		"SUN"
	};
	const char *const curr_dow = dow_strings [date.dow.positions.ones_pos];

	/* Display the date */
	lcd_data (date.date.positions.tens_pos + '0');
	lcd_data (date.date.positions.ones_pos + '0');

	/* Date-Month separator */
	lcd_data ('/');

	/* Display the month */
	lcd_data (date.month.positions.tens_pos + '0');
	lcd_data (date.month.positions.ones_pos + '0');

	/* Month-Year separator */
	lcd_data ('/');

	/* Display the year */
	lcd_data (date.year.positions.tens_pos + '0');
	lcd_data (date.year.positions.ones_pos + '0');

	/* Year-DOW separator */
	lcd_data (' ');

	/* Display the day of the week */
	lcd_data (*(curr_dow + 0));
	lcd_data (*(curr_dow + 1));
	lcd_data (*(curr_dow + 2));
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
		struct RTC_time time = { {0}, {0}, {0} };
		struct RTC_date date = { {0}, {0}, {0}, {0} };

		if ( RTC_read_time (&time) )
		{
			/* Glow all LEDs to indicate ACK failure and exit */
			PORTB = 0x00;
			return 1;
		}
		else
		{
			lcd_goto_line_home (1);
			display_time (time);
		}

		if ( RTC_read_date (&date) )
		{
			/* Glow all LEDs to indicate ACK failure and exit */
			PORTB = 0x00;
			return 1;
		}
		else
		{
			lcd_goto_line_home (2);
			display_date (date);
		}
	}

	return 0;
}
