/**
 * Simple program to read the seconds from the RTC chip in
 * the ATMEGA32 microcontroller.
 */

#include <avr/io.h>

/* Debug */
#define F_CPU 1000000UL
#include <util/delay.h>

#include "rtc/rtc.h"
#include "../lcd_display/lcd/lcd.h"

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
