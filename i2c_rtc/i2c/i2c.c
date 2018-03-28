#define F_CPU 1000000ul
#include <util/delay.h>
#include "i2c.h"

/*
 * Define constants for time periods.
 * Values are in micro seconds (us).
 */
#define CLK_PERIOD 20u
#define CLK_HIGH_PERIOD 10u /* CLK_PERIOD/2 */
#define CLK_HALF_HIGH_PERIOD 5u /* CLK_HIGH_PERIOD/2 */
#define CLK_LOW_PERIOD 10u /* CLK_PERIOD/2 */
#define CLK_HALF_LOW_PERIOD 5u /* CLK_LOW_PERIOD/2 */
#define STOP_START_FREE_TIME 5u

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

	/* Ensure SDA is configured for output
	 * when the SCL PIN IS LOW
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

	/* Ensure SDA is configured for output
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
}

void I2C_send_byte (uint8_t byte)
{
	static const uint8_t MSB_offset = 7;
	int8_t bit = MSB_offset;

	for (; bit >= 0; bit--)
	{
		I2C_send_bit ((byte & (1<<bit)) >> bit);
	}
}

void
I2C_send_ack (enum I2C_ack ack)
{
	/*
	 * Take advantage of the fact that the value of
	 * the ACK constant corresponds to the bit
	 * to be sent over the I2C channel.
	 */
	I2C_send_bit (ack);
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

	/* Ensure SDA is configured for input from slave
	 * when the SCL IS LOW
	 */
	DDR &= ~(1<<SDA_PIN);

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

uint8_t
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

/**
 * I2C_receive_ack:
 *
 * Returns values from the I2C_ack enumeration corresponding to the ACK received
 * from the I2C channel.
 *
 * Returns: values from the 'I2C_ack' enumeration
 */
enum I2C_ack
I2C_receive_ack (void)
{
	/*
	 * Take advantage of the fact that the value of
	 * the bit received corresponds to the
	 * value of the corresponding ACK constants.
	 */
	return I2C_receive_bit();
}
