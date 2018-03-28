#include"lcd.h"
#include <util/delay.h>

/**
 * Notes:
 *
 * 	R: 1
 * 	W: 0
 *
 * 	DB: Data Bus of LCD
 */

void lcd_command (uint8_t cmd)
{
	/*
	 * EN (0): 1
	 * RW (1): 0
	 * RS (2): 0
	 */
	PORTA = 0x01;

	/* write command */
	PORTD = cmd;

	/* clear all pins */
	PORTA = 0x00;

	/* wait for some time */
	_delay_ms(2);
}

void lcd_data (uint8_t data)
{
	/*
	 * EN (0): 1
	 * RW (1): 0
	 * RS (2): 1
	 */
	PORTA = 0x05;

	/* write data */
	PORTD = data;

	/* clear all pins */
	PORTA = 0x00;

	/* wait for some time */
	_delay_us (100);
}

void lcd_goto_line_home (uint8_t line)
{
	if (line == 0 || line > 2)
	{
		/* Do nothing if the request is for an invalid line */
		return;
	}

	if (line == 1)
	{
		/* 7-bit Home address: 0000000 */
		lcd_command (0x80);
	}
	else
	{
		/* 7-bit Home address: 1000000 */
		lcd_command (0xC0);
	}
}

void lcd_read_bf (void)
{
	/*
	 * EN (0): 1
	 * RW (1): 1
	 * RS (2): 0
	 */
	PORTA = 0x03;

	/* Pull down enable */
	PORTA &= 0xFE;
}

void initialize_lcd(void)
{
	/* Initialization sequence */
	/* 1. Initial wait for more than 15ms */
	_delay_ms (20u);

	/* 2. Write initialization specific data to pins (as per data sheet of LCD) */
	lcd_command (0x30);

	/* 3. Wait for more than 4.1ms */
	_delay_ms (5u);

	/* 4. Write initialization specific data to pins (as per data sheet of LCD) */
	lcd_command (0x30);

	/* 5. Wait for more than 100us (micro seconds) */
	_delay_us (150u);

	/* 6. Write initialization specific data to pins (as per data sheet of LCD) */
	lcd_command (0x30);

	/* 7. Initialization instructions */
	/*
	 * Function set
	 *
	 * DB7: 0
	 * DB6: 0
	 * DB5: 1
	 * DB4: 1
	 * DB3: 1 (N: 2 lines of display)
	 * DB2: 1 (F: 5*10 dots)
	 * DB1: DON'T CARE
	 * DB0: DON'T CARE
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
