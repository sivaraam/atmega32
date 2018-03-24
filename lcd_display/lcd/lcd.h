#ifndef KS_LCD_ATMEGA32
#define KS_LCD_ATMEGA32

/*
 * Helper functions to interface an LCD using a ATMEL ATMEGA32.
 *
 * Note that it is required to define the ports used for the
 * special pins  (EN, RS, RW) and data pins of the LCD before
 * including this header. Also note that, the ports need to be
 * distinct.
 *
 * Port configurations:
 *
 * PORTD (0-7) - data pins to LCD
 * PORTA:
 *
 * 	Pin 0: Enable pin of LCD
 * 	Pin 1: Read/Write pin of LCD
 * 	Pin 2: RS pin of LCD
 *
 * Notes:
 *
 * 1."All pins" of the above mentioned ports have to be initialized
 *    for output before invoking any of the functions.
 *
 * 2. As the functions internally use the _delay_ms and _delay_us
 *    functions of 'util/delay.h' (a.k.a avr/delay.h), F_CPU is
 *    defined to the default clock source of 1MHz.
 *
 *    There is currently no way around to redefine the clock rate
 *    except modifying this header.
 */

#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL

/**
 * lcd_command:
 *
 * @cmd: The command to be sent to hte LCD
 *
 * Send the comamand to the LCD by intializing the special
 * pins of the LCD as required for sending the command and
 * sending the command itself through the data pins.
 *
 * Note that the functions waits for a specific amount of time
 * required for the LCD to process the command[1] and returns only
 * after that.
 *
 * [1]: Currently, regardless of the command being issued, the function
 *      waits for the maximum amount of time required to execute any
 *      command on the LCD.
 */
void lcd_command (uint8_t cmd);

/**
 * lcd_data:
 *
 * @data: The data to be written to the DD RAM of the LCD.
 *
 * Send the display data to the LCD by intializing the special
 * pins of the LCD as required for writing data and sending the
 * data through the data pins.
 *
 * Note that the functions waits for a specific amount of time
 * required for the LCD to process the data and returns only after
 * that.
 */
void lcd_data (uint8_t data);

/**
 * initialize_lcd:
 *
 * Initialize the LCD by following the intialization sequence given in
 * the data sheet of the LCD.
 *
 * Also, configure the display correctly after completing the intialization
 * sequence.
 */
void initialize_lcd(void);

#endif
