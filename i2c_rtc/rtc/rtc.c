#include "../i2c/i2c.h"
#include "rtc.h"

static const uint8_t rtc_slave_addr__write = 0xD0,
                     rtc_slave_addr__read  = 0xD1;

static const uint8_t seconds_register_addr = 0x00;

int8_t
RTC_init (void)
{
	/* Initialize the port pins used by I2C */
	I2C_init();

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
