/*
 * lcd1602.h
 *
 *  Created on: Aug 8, 2026
 *      Author: dev
 */

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

#include"main.h"


#define LCD1602_POWER_ON_DELAY_MS       40U
#define LCD1602_COMMAND_DELAY_MS         2U
#define LCD1602_CLEAR_HOME_DELAY_MS      2U
#define LCD1602_EXECUTION_DELAY_MS       1U


#define LCD_CMD_CLEAR_DISPLAY      0x01U
#define LCD_CMD_RETURN_HOME        0x02U
#define LCD_CMD_ENTRY_MODE_SET     0x04U
#define LCD_CMD_DISPLAY_CONTROL    0x08U
#define LCD_CMD_CURSOR_SHIFT       0x10U
#define LCD_CMD_FUNCTION_SET       0x20U
#define LCD_CMD_SET_CGRAM_ADDR     0x40U
#define LCD_CMD_SET_DDRAM_ADDR     0x80U

typedef enum{
	LCD_INTERFACE_GPIO,
	LCD_INTERFACE_I2C
}LCD_Interface_t;


typedef enum {
	LCD_MODE_4BIT,
	LCD_MODE_8BIT
}LCD_Mode_t;

typedef enum
{
    LCD_LINES_1 = 1,
    LCD_LINES_2 = 2
} LCD_Lines_t;

typedef enum
{
    LCD_FONT_5x8,
    LCD_FONT_5x10
} LCD_Font_t;


typedef enum
{
    LCD_DISPLAY_OFF,
    LCD_DISPLAY_ON
} LCD_DisplayState_t;

typedef enum
{
    LCD_CURSOR_OFF,
    LCD_CURSOR_ON
} LCD_CursorState_t;


typedef enum
{
    LCD_BLINK_OFF,
    LCD_BLINK_ON
} LCD_BlinkState_t;

typedef enum{
	LCD_BACKLIGHT_OFF,
	LCD_BACKLIGHT_ON
}LCD_BackLightState_t;

typedef enum{
	LCD_OK = 0,
	LCD_ERROR,
	LCD_TIMEOUT,
	LCD_INVALID_PARAM
}LCD_Status_t;


typedef struct{

	LCD_Interface_t interface;
	LCD_Mode_t mode;

	LCD_Lines_t lines;
	LCD_Font_t font;

	LCD_BlinkState_t blink;
	LCD_CursorState_t cursor;
	LCD_DisplayState_t display;

	uint8_t columns;
	LCD_BackLightState_t backLight;

	I2C_HandleTypeDef *hi2c;
	uint8_t i2c_address;

}LCD_Handle_t;


LCD_Status_t LCD_Init(LCD_Handle_t *lcd);

LCD_Status_t LCD_Clear(LCD_Handle_t *lcd);

LCD_Status_t LCD_Home(LCD_Handle_t *lcd);

LCD_Status_t LCD_SetCursor(LCD_Handle_t *lcd, uint8_t row , uint8_t column);

LCD_Status_t LCD_WriteChar(LCD_Handle_t *lcd, char data);

LCD_Status_t LCD_WriteString(LCD_Handle_t *lcd, const char *str);

LCD_Status_t LCD_DisplayOn(LCD_Handle_t *lcd);

LCD_Status_t LCD_DisplayOff(LCD_Handle_t *lcd);

LCD_Status_t LCD_CursorOn(LCD_Handle_t *lcd);

LCD_Status_t LCD_CursorOff(LCD_Handle_t *lcd);

LCD_Status_t LCD_BlinkOn(LCD_Handle_t *lcd);

LCD_Status_t LCD_BlinkOff(LCD_Handle_t *lcd);

LCD_Status_t LCD_ShiftDisplayLeft(LCD_Handle_t *lcd);

LCD_Status_t LCD_ShiftDisplayRight(LCD_Handle_t *lcd);

LCD_Status_t LCD_ShiftCursorLeft(LCD_Handle_t *lcd);

LCD_Status_t LCD_ShiftCursorRight(LCD_Handle_t *lcd);

LCD_Status_t LCD_CreateChar(LCD_Handle_t *lcd,
                            uint8_t location,
                            const uint8_t pattern[8]);
LCD_Status_t LCD_BacklightOn(LCD_Handle_t *lcd);
LCD_Status_t LCD_BacklightOff(LCD_Handle_t *lcd);

#endif /* INC_LCD1602_H_ */
