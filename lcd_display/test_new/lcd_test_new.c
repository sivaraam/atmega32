/**
 * Program to test the how to interface with an LCD.
 *
 * Port C - data pins to LCD
 * Port D:
 *
 * 	Pin 0: Enable pin of LCD
 * 	Pin 1: Read/Write pin of LCD
 * 	Pin 2: RS pin of LCD
 *
 * Notes:
 *
 * 	R: logic 1
 * 	W: logic 0
 *
 * 	DB: Data Bus of LCD
 */

#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#define F_CPU 1000000UL
#include <util/delay.h>

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
void lcd_command (uint8_t cmd)
{
	/*
	 * EN (0): 1
	 * RW (1): 0
	 * RS (2): 0
	 */
	PORTA = 0x01;

	// write command
	PORTD = cmd;

	// clear all pins
	PORTA = 0x00;

	// wait for some time
	_delay_ms(2);
}

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
void lcd_data (uint8_t data)
{
	/*
	 * EN (0): 1
	 * RW (1): 1
	 * RS (2): 0
	 */
	PORTA = 0x05;

	// write data
	PORTD = data;

	// clear all pins
	PORTA = 0x00;

	// wait for some time
	_delay_us (100);
}

/**
 * initialize_lcd:
 *
 * Initialize the LCD by following the intialization sequence given in
 * the data sheet of the LCD.
 *
 * Also, configure the display correctly after completing the intialization
 * sequence.
 */
void initialize_lcd(void)
{
	// Initialization sequence
	/**
	 * Initial wait for more than 15ms
	 */
	_delay_ms (20u);

	// 2. Write initialization specific data to pins (as per data sheet of LCD)
	lcd_command (0x30);

	// 3. Wait for more than 4.1ms
	_delay_ms (5u);

	// 4. Write initialization specific data to pins (as per data sheet of LCD)
	lcd_command (0x30);

	// 5. Wait for more than 100us (micro seconds)
	_delay_us (150u);

	// 6. Write initialization specific data to pins (as per data sheet of LCD)
	lcd_command (0x30);

	// 7. Initialization instructions
	/*
	 * Function set
	 *
	 * DB7: 0
	 * DB6: 0
	 * DB5: 1
	 * DB4: 1
	 * DB3: 1 (N: 2 lines of display)
	 * DB2: 1 (F: 5*10 dots)
	 * DB1: DONT CARE
	 * DB0: DONT CARE
	 */
	lcd_command (0x3C);

	/**
	 * Display OFF
	 */
	lcd_command (0x08);

	/**
	 * Clear display
	 */
	lcd_command (0x01);

	/**
	 * Entry mode set:
	 *
	 * DB7: 0
	 * DB6: 0
	 * DB5: 0
	 * DB4: 0
	 * DB3: 0
	 * DB2: 1
	 * DB1: 1 (I/D: increment)
	 * DB0: 0 (S: Display shift OFF)
	 */
	lcd_command (0x06);

	/**
	 * Display set
	 *
	 * DB7: 0
	 * DB6: 0
	 * DB5: 0
	 * DB4: 0
	 * DB3: 1
	 * DB2: 1 (D: Display ON)
	 * DB1: 0 (C: Cursor OFF)
	 * DB0: 0 (B: Blink cursor OFF)
	 */
	lcd_command (0x0C);
}

void write_data (const char *const data)
{
	for (uint16_t curr_char=0; curr_char < strlen (data); curr_char++)
	{
		lcd_data (*(data+curr_char));

	}
}

int main(void)
{
	/**
	 * Initialize ports used for LCD as outputs.
	 */
	DDRD = 0xFF;

	/**
	 * Note that only three pins of PORT A are actually
	 * used to connect to the LCD but all ports are initialized
	 * to simplify changing the value of the three pins.
	 */
	DDRA = 0xFF;

	initialize_lcd();

	write_data ("Hello world!");
}
