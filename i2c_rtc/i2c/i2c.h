#ifndef KS_I2C_HELPERS
#define KS_I2C_HELPERS

/**
 * Simple implementation of helper functions that enable I2C
 * communication. This is done for a ATMEGA32 microcontroller
 * whose clock frequency is 1MHz.
 *
 * The SDA and SCL lines are handled manually. No internal I2C helping
 * functions are used for the I2C communication. Therefore, the I2C clock
 * is generated my manually toggling the SCL line as and when required.
 * So, the clock might not be at a precise frequency.
 *
 * Notes:
 *
 * - Every function would execute for one clock period doing the
 *   required toggling as required.
 *
 * - The clock frequency for SCL of I2C: 50kHz (approx.)
 *
 * - Free time between a STOP and START: 5us
 *
 * - Hold time for START: 5us
 *
 * - Configuring the direction of the pins used for the communication
 *   is done as and when required by the library internally.
 *
 * Pins:
 *
 * 	PORT:
 *
 * 		0 - SCL
 * 		1 - SDA
 */

#include <stdint.h>
#include <avr/io.h>

#define PORT PORTA
#define PIN PINA
#define DDR DDRA

/*
 * Define constants for clock pin offsets
 */
#define SCL_PIN 0
#define SDA_PIN 1

/**
 * Constants that represent values corresponding to the ACK received.
 */
enum I2C_ack
{
	I2C_ACK_ACK = 4,
	I2C_ACK_NACK = 7
};

/**
 * I2C_init:
 *
 * Initialise I2C communication by initialising the SCL and SDA pins.
 */
void
I2C_init (void);

/**
 * I2C_start:
 *
 * Start the I2C communication by sending the start signal. The start signal
 * is to toggle the SDA from high to low during the high clock period.
 */
void
I2C_start (void);

/**
 * I2C_stop:
 *
 * Stop the I2C communication by sending the stop signal. The stop signal is to
 * toggle the SDA from low to high during the high clock period.
 */
void
I2C_stop (void);

/**
 * I2C_send_byte:
 *
 * @byte: the byte to be sent
 *
 * Send a byte over the I2C channel.
 *
 * Note: Transfer is done MSB first as specified by I2C.
 */
void I2C_send_byte (uint8_t byte);

/**
 * I2C_send_ack:
 *
 * @ack: the ACK to be sent over the I2C channel.
 *
 * Sends the requested ACK over the I2C channel.
 *
 */
void
I2C_send_ack (enum I2C_ack ack);

/**
 * I2C_receive_byte:
 *
 * Return the byte that was read during 8 high clock periods.
 *
 * Note: Data is expected to be received MSB first as specified by I2C.
 *
 * Returns: a byte that was read from the I2C channel.
 */
uint8_t
I2C_receive_byte (void);

/**
 * I2C_receive_ack:
 *
 * Returns values from the I2C_ack enumeration corresponding to the ACK received
 * from the I2C channel.
 *
 * Returns: values from the 'I2C_ack' enumeration
 */
enum I2C_ack
I2C_receive_ack (void);

#endif
