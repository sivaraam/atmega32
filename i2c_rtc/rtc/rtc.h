#ifndef KS_RTC_I2C
#define KS_RTC_I2C

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
 * their custom default values.
 *
 * Returns: 0 if the initialization was successful. A non-zero value
 *          in case the initialization failed in some case.
 */
int8_t
RTC_init (void);

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
RTC_read_time (struct RTC_time *time);

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
RTC_read_date (struct RTC_date *date);

#endif
