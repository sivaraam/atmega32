#define F_CPU 1000000ul
#include <util/delay.h>
#include "i2c.h"

/**
 * Implementation note:
 *
 * Data direction:
 *
 * 	- The data direction of SCL is always set for output.
 *
 * 	- As the amount of data read through I2C is expected to
 * 	  be more as compared to the amount of data written using
 * 	  I2C the data direction of the SDA line is always
 * 	  expected to be set for input.
 *
 * 	  So, the helper functions related to reading data do not
 * 	  change the data direction. The helper functions related
 * 	  to writing data change the data direction for output and
 * 	  change it back to input after the required work has been done.
 *
 * 	  This would possibly reduce the unwanted data direction changes.
 *
 * 	- The data direction is to be toggled only when the clock is low
 * 	  otherwise it would lead to bad consequences while reading the
 * 	  acknowledgement.
 *
 * Clock:
 *
 * 	- Every helper function at a minimum would toggle the clock for
 * 	  one complete clock cycle.
 */

/*
 * Define constants for time periods.
 * Values are in micro seconds (us).
 */
#define CLK_PERIOD 20u
#define CLK_HIGH_PERIOD (CLK_PERIOD/2)
#define CLK_HALF_HIGH_PERIOD (CLK_HIGH_PERIOD/2)
#define CLK_LOW_PERIOD (CLK_PERIOD/2)
#define CLK_HALF_LOW_PERIOD (CLK_LOW_PERIOD/2)
#define STOP_START_FREE_TIME 5

typedef void (*delay) (double period);

static delay delay_fn = _delay_us;

inline void
I2C_init (void)
{
	/* Initially configure the SDA and SCL for output
	 * when the SCL IS LOW
	 */
	DDR |= (1<<SCL_PIN) | (1<<SDA_PIN);
	PORT |= (1<<SCL_PIN) | (1<<SDA_PIN);
}

/**
 * I2C_start_stop_helper:
 *
 * Helper function to avoid redundancy in the start and stop functions.
 */
static void
I2C_start_stop_helper (_Bool start)
{
	/* Initially keep the SCL low */
	PORT &= ~(1<<SCL_PIN);

	/* Change data direction of SDA to output
	 * when the SCL IS LOW
	 */
	DDR |= (1<<SDA_PIN);

	/* Wait for half the clock low period */
	delay_fn (CLK_HALF_LOW_PERIOD);

	/* Ensure SDA is at the level required for the desired condition */
	(start) ? (PORT |= (1<<SDA_PIN)) :
	          (PORT &= ~(1<<SDA_PIN));

	/* Wait for rest of the clock low period */
	delay_fn (CLK_HALF_LOW_PERIOD);

	/* Keep SCL high for half the clock high period */
	PORT |= (1<<SCL_PIN);
	delay_fn (CLK_HALF_HIGH_PERIOD);

	/* Toggle SDA */
	(start) ? (PORT &= ~(1<<SDA_PIN)):
	          (PORT |= (1<<SDA_PIN));

	/* Wait for rest of the clock high period */
	delay_fn (CLK_HALF_HIGH_PERIOD);

	/* Change data direction back to input
	 * when the SCL IS LOW.
	 */
	PORT &= ~(1<<SCL_PIN);
	DDR &= ~(1<<SDA_PIN);
}

/**
 * I2C_send_bit:
 *
 * @bit: The last bit (LSB) corresponds to the bit to be sent
 *
 * Send a normal bit over the I2C channel.
 */
static inline void
I2C_send_bit (uint8_t bit)
{
	/* Initially keep the SCL low for half the clock low period */
	PORT &= ~(1<<SCL_PIN);

	/* Change data direction of SDA to output
	 * when the SCL IS LOW
	 */
	DDR |= (1<<SDA_PIN);

	delay_fn (CLK_HALF_LOW_PERIOD);

	/* Set SDA to the bit to be sent */
	(bit & 1) ? (PORT |= (1<<SDA_PIN)) :
	            (PORT &= ~(1<<SDA_PIN));

	/* Wait for rest of the clock low period */
	delay_fn (CLK_HALF_LOW_PERIOD);

	/* Keep SCL high for the clock high period */
	PORT |= (1<<SCL_PIN);
	delay_fn (CLK_HIGH_PERIOD);

	/* Change data direction back to input
	 * when the SCL IS LOW.
	 */
	PORT &= ~(1<<SCL_PIN);
	DDR &= ~(1<<SDA_PIN);
}

/**
 * I2C_send_ack:
 *
 * @ack: the ACK to be sent over the I2C channel.
 *
 * Sends the requested ACK over the I2C channel.
 *
 */
static inline void
I2C_send_ack (uint8_t ack)
{
	/*
	 * Take advantage of the fact that the value of
	 * the ACK constant corresponds to the bit
	 * to be sent over the I2C channel.
	 */
	I2C_send_bit (ack);
}

/**
 * I2C_send_byte:
 *
 * @byte: the byte to be sent
 *
 * Send a byte over the I2C channel.
 */
static void
I2C_send_byte (uint8_t byte)
{
	static const uint8_t MSB_offset = 7;
	int8_t bit = MSB_offset;

	for (; bit >= 0; bit--)
	{
		I2C_send_bit ((byte & (1<<bit)) >> bit);
	}
}

/**
 * I2C_receive_bit:
 *
 * Return the bit that was obtained during the high period of the clock.
 *
 * Returns: a byte whose LSB represents the data that was received.
 *          Other bits are 0.
 */
static inline uint8_t
I2C_receive_bit (void)
{
	uint8_t input_bit = 0;

	/* Initially pull SCL to low */
	PORT &= ~(1<<SCL_PIN);

	/* Keep SCL low for the clock low period
	 * allowing slave to toggle SDA as required
	 */
	delay_fn (CLK_LOW_PERIOD);

	/* Pull the clock high and wait for half the clock
	 * high period before reading the value
	 */
	PORT |= (1<<SCL_PIN);
	delay_fn (CLK_HALF_HIGH_PERIOD);

	input_bit = (PIN & (1<<SDA_PIN)) >> SDA_PIN;

	/* Wait for rest of the clock high period */
	delay_fn (CLK_HALF_HIGH_PERIOD);

	return input_bit;
}

/**
 * I2C_receive_ack:
 *
 * Returns constants whose values correspond to the ACK received
 * from the I2C channel.
 *
 * Returns: constants I2C_ACK_ACK or I2C_ACK_NACK corresponding to the
 *          acknowledgement received over the I2C channel.
 */
static inline uint8_t
I2C_receive_ack (void)
{
	/*
	 * Take advantage of the fact that the value of
	 * the bit received corresponds to the
	 * value of the corresponding ACK constants.
	 */
	return I2C_receive_bit();
}

/**
 * I2C_receive_byte:
 *
 * Return the byte that was read during 8 high clock periods.
 *
 * Note: Data is expected to be received MSB first as specified by I2C.
 *
 * Returns: a byte that was read from the I2C channel.
 */
static uint8_t
I2C_receive_byte (void)
{
	static const uint8_t MSB_offset = 7;
	uint8_t input_byte = 0;
	int8_t bit = MSB_offset;

	for (; bit >= 0; bit--)
	{
		const uint8_t input_bit = I2C_receive_bit();
		input_byte = input_byte | (input_bit << bit);
	}

	return input_byte;
}

void
I2C_start (void)
{
	I2C_start_stop_helper (1);
}

void
I2C_stop (void)
{
	I2C_start_stop_helper (0);
	delay_fn (STOP_START_FREE_TIME);
}

int8_t
I2C_send (uint8_t byte)
{
	I2C_send_byte (byte);

	/*
	 * Take advantage of the fact that the value of
	 * the ACK constant match with the corresponding
	 * values to be returned.
	 */
	return I2C_receive_ack();
}

uint8_t
I2C_receive (uint8_t ack_to_send)
{
	uint8_t incoming_byte = I2C_receive_byte ();

	I2C_send_ack (ack_to_send);

	return incoming_byte;
}
