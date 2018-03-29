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
 * 		6 - SCL
 * 		7 - SDA
 */

#include <stdint.h>
#include <avr/io.h>

#define PORT PORTA
#define PIN PINA
#define DDR DDRA

/*
 * Define constants for clock pin offsets
 */
#define SCL_PIN 6
#define SDA_PIN 7

/**
 * Constants that represent values corresponding to the ACK received.
 */
#define I2C_ACK_ACK 0u
#define I2C_ACK_NACK 1u

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
 * I2C_send:
 *
 * @byte: byte to be sent
 *
 * Send a byte to the I2C channel and check for the ACK
 * received for the corresponding byte.
 *
 * Note: Transfer is done MSB first as specified by I2C.
 *
 * Returns: 0 if an ACK is received else returns a non-zero
 *          value.
 */
int8_t
I2C_send (uint8_t byte);

/**
 * I2C_receive:
 *
 * @ack_to_send: the ack value to send after the corresponding receive
 *
 * Receive a byte from the I2C channel and send the given ACK after the
 * receive.
 *
 * Note: An ACK signal the transmitter that the byte was successfully received
 *       and another byte may be sent as specified by I2C. An NACK does not mean
 *       much to the transmitter when it is the slave. In general the slave would
 *       stop transmitting when an NACK is received. For a master, NACK means
 *       the master can then generate either a STOP condition to abort the
 *       transfer, or a repeated START condition to start a new transfer.
 */
uint8_t
I2C_receive (uint8_t ack_to_send);

#endif
