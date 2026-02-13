/*
 * lcd.c
 *
 *  Created on: Feb 5, 2026
 *      Author: G45
 */

#include <lib_lcd.h>
#include "usart.h"
#include "main.h"
//#include "string.h"
#include "i2c.h"


// LCD initialization function (LCD + RGB backlight)
void lcd_init(I2C_HandleTypeDef* I2Cx, rgb_lcd* DataStruct)
{
	I2C_HandleTypeDef* Handle = I2Cx;

	// Configure display function: 2 lines, 5x10 dots font
	DataStruct->_displayfunction |= LCD_2LINE | LCD_5x10DOTS;

	uint8_t data[2];
	data[0] = 0x80; // Command register
	data[1] = LCD_FUNCTIONSET | DataStruct->_displayfunction;

	// Debug: send command over UART
	HAL_UART_Transmit(&huart2,(uint8_t *)data,2,10);
	char newline[2] = "\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) newline, 2, 10);

	// Check if LCD device is ready on I2C
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(Handle,LCD_ADDRESS,5,100);
	HAL_UART_Transmit(&huart2,&status,1,10);
	HAL_Delay(50);

	// Send function set command to LCD
	HAL_StatusTypeDef status2 = HAL_I2C_Master_Transmit(Handle, LCD_ADDRESS, data,2,5000);
	HAL_UART_Transmit(&huart2,&status2,1,10);
	HAL_Delay(50);

	// Repeat command as part of initialization sequence
	HAL_I2C_Master_Transmit(Handle,LCD_ADDRESS,data,2,5000);
	HAL_Delay(5);
	HAL_I2C_Master_Transmit(Handle,LCD_ADDRESS,data,2,5000);

	// DISPLAY CONTROL: turn display ON, cursor OFF, blink OFF
	DataStruct->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	data[1] = LCD_DISPLAYCONTROL | DataStruct->_displaycontrol;
	HAL_I2C_Master_Transmit(Handle,LCD_ADDRESS, data,2,1000);

	// Clear display
	data[1] = LCD_CLEARDISPLAY;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)LCD_ADDRESS, (uint8_t *)data,2,1000);
	HAL_Delay(2);

	// ENTRY MODE: text direction left-to-right, no display shift
	DataStruct->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	data[1] = LCD_ENTRYMODESET | DataStruct->_displaymode;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)LCD_ADDRESS, (uint8_t *)data,2,1000);

	// -------- RGB Backlight Initialization --------

	uint8_t data_backlight[2];

	// Set MODE1 register
	data_backlight[0] = REG_MODE1;
	data_backlight[1] = 0;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_backlight,2,1000);

	// Enable output
	data_backlight[0] = REG_OUTPUT;
	data_backlight[1] = 0xFF;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_backlight,2,1000);

	// Set MODE2 register
	data_backlight[0] = REG_MODE2;
	data_backlight[1] = 0x20;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_backlight,2,1000);

	// Set initial LCD color to white (R=255, G=255, B=255)
	uint8_t data_rgb[2];

	data_rgb[0] = REG_RED;
	data_rgb[1] = 255;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_rgb,2,1000);

	data_rgb[0] = REG_GREEN;
	data_rgb[1] = 255;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_rgb,2,1000);

	data_rgb[0] = REG_BLUE;
	data_rgb[1] = 255;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_rgb,2,1000);
}


// Clear the LCD by overwriting both lines with spaces
void clearlcd(void)
{
	lcd_position(&hi2c1,0,0);
	lcd_print(&hi2c1,"                "); // 16 spaces
	lcd_position(&hi2c1,0,1);
	lcd_print(&hi2c1,"                ");
}


// Write a single character (value) to the LCD
void lcd_write(I2C_HandleTypeDef* I2Cx, uint8_t value)
{
	I2C_HandleTypeDef* Handle = I2Cx;

    // 0x40 selects data register (character write)
    unsigned char data[2] = {0x40, value};

    HAL_I2C_Master_Transmit(Handle,LCD_ADDRESS, data,2,1000);
}


// Print a null-terminated string to the LCD
void lcd_print(I2C_HandleTypeDef* I2Cx, char *str)
{
	I2C_HandleTypeDef* Handle = I2Cx;

    char data[2];
    data[0] = 0x40; // Data register

    int i=0;
    while(str[i] != '\0')
    {
		data[1] = str[i];
		HAL_I2C_Master_Transmit(Handle,LCD_ADDRESS,(uint8_t *) data, 2,1000);
		i++;
   }
}


// Set cursor position (column and row)
void lcd_position(I2C_HandleTypeDef* I2Cx,char col, char row)
{
	I2C_HandleTypeDef* Handle = I2Cx;

    // Row 0 starts at address 0x80, row 1 at 0xC0
    if(row == 0)
    {
        col = col | 0x80;
    }
    else
    {
        col = col | 0xc0;
    }

    char data[2];
    data[0] = 0x80; // Command register
    data[1] = col;

    HAL_I2C_Master_Transmit(Handle,LCD_ADDRESS,(uint8_t *) data, 2,1000);
}


// Set RGB backlight color
void reglagecouleur(uint8_t R,uint8_t G,uint8_t B)
{
	I2C_HandleTypeDef* Handle = &hi2c1;

	uint8_t data_r[2];
	uint8_t data_g[2];
	uint8_t data_b[2];

	// Set red intensity
	data_r[0] = REG_RED;
	data_r[1] = R;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_r,2,1000);

	// Set green intensity
	data_g[0] = REG_GREEN;
	data_g[1] = G;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_g,2,1000);

	// Set blue intensity
	data_b[0] = REG_BLUE;
	data_b[1] = B;
	HAL_I2C_Master_Transmit(Handle, (uint16_t)RGB_ADDRESS, (uint8_t *)data_b,2,1000);
}


// Generic I2C transmit function (currently fixed length = 2 bytes)
void variable_Transmit(uint16_t address, uint8_t *Data, uint16_t len)
{
	 // NOTE: len parameter is ignored (always sends 2 bytes)
	 HAL_I2C_Master_Transmit(&hi2c1, address, Data, 2, 1000);
}
