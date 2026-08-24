/*
 * lcd1602.c
 *
 *  Created on: Aug 8, 2026
 *      Author: dev
 */
#include "lcd1602.h"
#include "lcd1602_port.h"


/* GPIO interface */
static void LCD_GPIO_RS_High(void){
	HAL_GPIO_WritePin(LCD1602_RS_PORT,
                      LCD1602_RS_PIN,
                      GPIO_PIN_SET);
}
static void LCD_GPIO_RS_Low(void){
	HAL_GPIO_WritePin(LCD1602_RS_PORT,
                      LCD1602_RS_PIN,
                      GPIO_PIN_RESET);
}

static void LCD_GPIO_EN_High(void){
	HAL_GPIO_WritePin(LCD1602_EN_PORT,
                      LCD1602_EN_PIN,
                      GPIO_PIN_SET);
}

static void LCD_GPIO_EN_Low(void){
    HAL_GPIO_WritePin(LCD1602_EN_PORT,
                      LCD1602_EN_PIN,
                      GPIO_PIN_RESET);
}

static void LCD_GPIO_PulseEnable(void){
	    LCD_GPIO_EN_High();

	    /* Enable pulse width */
	    for (volatile uint32_t i = 0; i < 10U; i++)
	    {
	        __NOP();
	    }

	    LCD_GPIO_EN_Low();

}

static void LCD_GPIO_WriteNibble(uint8_t nibble){

	    HAL_GPIO_WritePin(LCD1602_D4_PORT, LCD1602_D4_PIN , (nibble >> 0) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D5_PORT, LCD1602_D5_PIN , (nibble >> 1) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D6_PORT, LCD1602_D6_PIN , (nibble >> 2) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D7_PORT, LCD1602_D7_PIN , (nibble >> 3) & 0x01);
	    LCD_GPIO_PulseEnable();
}

static void LCD_GPIO_ClearDataLines(void){
    HAL_GPIO_WritePin(LCD1602_D0_PORT, LCD1602_D0_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD1602_D1_PORT, LCD1602_D1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD1602_D2_PORT, LCD1602_D2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD1602_D3_PORT, LCD1602_D3_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD1602_D4_PORT, LCD1602_D4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD1602_D5_PORT, LCD1602_D5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD1602_D6_PORT, LCD1602_D6_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD1602_D7_PORT, LCD1602_D7_PIN, GPIO_PIN_RESET);
}

static void LCD_GPIO_WriteByte_8Bit(uint8_t data){
	    HAL_GPIO_WritePin(LCD1602_D0_PORT, LCD1602_D0_PIN , (data >> 0) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D1_PORT, LCD1602_D1_PIN , (data >> 1) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D2_PORT, LCD1602_D2_PIN , (data >> 2) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D3_PORT, LCD1602_D3_PIN , (data >> 3) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D4_PORT, LCD1602_D4_PIN , (data >> 4) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D5_PORT, LCD1602_D5_PIN , (data >> 5) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D6_PORT, LCD1602_D6_PIN , (data >> 6) & 0x01);
	    HAL_GPIO_WritePin(LCD1602_D7_PORT, LCD1602_D7_PIN , (data >> 7) & 0x01);

	    LCD_GPIO_PulseEnable();
}

static void LCD_GPIO_WriteByte_4Bit(uint8_t data){
    LCD_GPIO_WriteNibble(data >> 4);   // Higher 4 bits
    LCD_GPIO_WriteNibble(data & 0x0F); // Lower 4 bits
}


static LCD_Status_t LCD_GPIO_SendByte(LCD_Handle_t *lcd , uint8_t data){

	if(lcd->mode == LCD_MODE_4BIT){
		LCD_GPIO_WriteByte_4Bit(data);
		return LCD_OK;
	}

	if(lcd->mode == LCD_MODE_8BIT){
		LCD_GPIO_WriteByte_8Bit(data);
		return LCD_OK;
	}

	return LCD_INVALID_PARAM;
}


static LCD_Status_t LCD_I2C_Write(LCD_Handle_t *lcd, uint8_t data){

    HAL_StatusTypeDef status;

    if (lcd->hi2c == NULL)
    {
        return LCD_INVALID_PARAM;
    }

    status = HAL_I2C_Master_Transmit(lcd->hi2c,
    		                        (uint16_t)(lcd->i2c_address << 1),
                                     &data,
                                     1U,
                                     100U);

    if (status == HAL_OK)
    {
        return LCD_OK;
    }

    if (status == HAL_TIMEOUT)
    {
        return LCD_TIMEOUT;
    }

    return LCD_ERROR;
}


static LCD_Status_t LCD_I2C_PulseEnable(LCD_Handle_t *lcd , uint8_t data){

	uint8_t enable_data;
	LCD_Status_t status;
	enable_data = data|(1U << LCD1602_EN_BIT);

	status = LCD_I2C_Write(lcd, enable_data);

	if(status != LCD_OK){
		return status;
	}

	 for (volatile uint32_t i = 0; i < 10U; i++)
	    {
	        __NOP();
	    }
	 enable_data = data & ~(1U << LCD1602_EN_BIT);

	 status = LCD_I2C_Write(lcd, enable_data);

	 return status;
}


static LCD_Status_t LCD_I2C_WriteNibble(LCD_Handle_t *lcd, uint8_t nibble, uint8_t rs){

	uint8_t data = 0U;

	if(rs){
		data |= (1U << LCD1602_RS_BIT);
	}

	data &= ~(1U << LCD1602_RW_BIT);

	if (lcd->backLight == LCD_BACKLIGHT_ON){
	    data |= (1U << LCD1602_BL_BIT);
	}


	data |=(((nibble >> 0)& 0x01U) << LCD1602_D4_BIT);
	data |=(((nibble >> 1)& 0x01U) << LCD1602_D5_BIT);
	data |=(((nibble >> 2)& 0x01U) << LCD1602_D6_BIT);
	data |=(((nibble >> 3)& 0x01U) << LCD1602_D7_BIT);

	return LCD_I2C_PulseEnable(lcd, data);
}


static LCD_Status_t LCD_I2C_SendByte(LCD_Handle_t *lcd , uint8_t data, uint8_t rs){

	LCD_Status_t status;

	if(lcd->mode != LCD_MODE_4BIT){
		return LCD_INVALID_PARAM;
	}

	status= LCD_I2C_WriteNibble(lcd, ((data >>4) & 0x0F), rs);

	if(status != LCD_OK){
		return status;
	}

	status= LCD_I2C_WriteNibble(lcd, (data &0x0F), rs);

	return status;
}



static LCD_Status_t LCD_SendCommand(LCD_Handle_t *lcd , uint8_t command){

	LCD_Status_t status;

	if(lcd->interface == LCD_INTERFACE_GPIO){
		LCD_GPIO_RS_Low();
		status = LCD_GPIO_SendByte(lcd, command);
		HAL_Delay(LCD1602_COMMAND_DELAY_MS );

		return status;
	}

	if(lcd->interface == LCD_INTERFACE_I2C){
		status = LCD_I2C_SendByte(lcd, command, 0U);
		HAL_Delay(LCD1602_COMMAND_DELAY_MS );

		return status;
	}

	return LCD_INVALID_PARAM;
}


static LCD_Status_t LCD_SendData(LCD_Handle_t *lcd , uint8_t data){

	LCD_Status_t status;
	if(lcd->interface == LCD_INTERFACE_GPIO){
		LCD_GPIO_RS_High();
		status= LCD_GPIO_SendByte(lcd, data);
		HAL_Delay(LCD1602_EXECUTION_DELAY_MS);
		return status;
	}

	if(lcd->interface == LCD_INTERFACE_I2C){
		return LCD_I2C_SendByte(lcd, data, 1U);
	}

	return LCD_INVALID_PARAM;
}

static LCD_Status_t LCD_ApplyDisplayConfig(LCD_Handle_t *lcd){

	LCD_Status_t status;
	uint8_t command = LCD_CMD_DISPLAY_CONTROL;


    if (lcd->display == LCD_DISPLAY_ON){
        command |= 0x04U;           // display on/off bit
    }

    if (lcd->cursor == LCD_CURSOR_ON){
        command |= 0x02U;
    }

    if (lcd->blink == LCD_BLINK_ON){
        command |= 0x01U;
    }

    status = LCD_SendCommand(lcd, command);

    return status;
}


LCD_Status_t LCD_DisplayOn(LCD_Handle_t *lcd){

    if (lcd == NULL)  {
        return LCD_INVALID_PARAM;
    }
    lcd->display = LCD_DISPLAY_ON;

    return LCD_ApplyDisplayConfig(lcd);
}

LCD_Status_t LCD_BlinkOn(LCD_Handle_t *lcd){

    if (lcd == NULL) {
        return LCD_INVALID_PARAM;
    }
    lcd->blink = LCD_BLINK_ON;

    return LCD_ApplyDisplayConfig(lcd);
}

LCD_Status_t LCD_CursorOn(LCD_Handle_t *lcd){

    if (lcd == NULL) {
        return LCD_INVALID_PARAM;
    }
    lcd->cursor = LCD_CURSOR_ON;

    return LCD_ApplyDisplayConfig(lcd);
}


LCD_Status_t LCD_DisplayOff(LCD_Handle_t *lcd){

    if (lcd == NULL) {
        return LCD_INVALID_PARAM;
    }
    lcd->display = LCD_DISPLAY_OFF;

    return LCD_ApplyDisplayConfig(lcd);
}

LCD_Status_t LCD_BlinkOff(LCD_Handle_t *lcd){

    if (lcd == NULL) {
        return LCD_INVALID_PARAM;
    }
    lcd->blink = LCD_BLINK_OFF;

    return LCD_ApplyDisplayConfig(lcd);
}

LCD_Status_t LCD_CursorOff(LCD_Handle_t *lcd){

    if (lcd == NULL){
        return LCD_INVALID_PARAM;
    }
    lcd->cursor = LCD_CURSOR_OFF;

    return LCD_ApplyDisplayConfig(lcd);
}


LCD_Status_t LCD_Clear(LCD_Handle_t *lcd){

	if (lcd == NULL) {
	        return LCD_INVALID_PARAM;
	    }

	LCD_Status_t status;

	status = LCD_SendCommand(lcd, LCD_CMD_CLEAR_DISPLAY);

	if (status == LCD_OK) {
	        HAL_Delay(LCD1602_CLEAR_HOME_DELAY_MS);
	    }
	return status;
}

LCD_Status_t LCD_Home(LCD_Handle_t *lcd){

	LCD_Status_t status;
	if (lcd == NULL) {
	        return LCD_INVALID_PARAM;
	    }

	status = LCD_SendCommand(lcd, LCD_CMD_RETURN_HOME);

	if(status == LCD_OK){
		HAL_Delay(LCD1602_CLEAR_HOME_DELAY_MS);
	}
	return status;
}


LCD_Status_t LCD_ShiftDisplayLeft(LCD_Handle_t *lcd){

	if (lcd == NULL) {
	        return LCD_INVALID_PARAM;
	    }

	return (LCD_SendCommand(lcd, (LCD_CMD_CURSOR_SHIFT|0x08U)));

}

LCD_Status_t LCD_ShiftDisplayRight(LCD_Handle_t *lcd){

	if (lcd == NULL) {
	        return LCD_INVALID_PARAM;
	    }
     return (LCD_SendCommand(lcd, (LCD_CMD_CURSOR_SHIFT|0x0CU)));
}


LCD_Status_t LCD_ShiftCursorLeft(LCD_Handle_t *lcd){

	if (lcd == NULL) {
	        return LCD_INVALID_PARAM;
	    }

	return (LCD_SendCommand(lcd, LCD_CMD_CURSOR_SHIFT));
}

LCD_Status_t LCD_ShiftCursorRight(LCD_Handle_t *lcd){

	if (lcd == NULL) {
	        return LCD_INVALID_PARAM;
	    }

	return (LCD_SendCommand(lcd, LCD_CMD_CURSOR_SHIFT|0x04U));
}


LCD_Status_t LCD_SetCursor(LCD_Handle_t *lcd, uint8_t row , uint8_t column){

	uint8_t address;

	if((lcd == NULL)||(row >= lcd->lines)||(column >= lcd->columns)){
		return LCD_INVALID_PARAM;
	}

	if(row == 0U){
		address = column;
	}

	else if(row == 1U){
		address = 0x40U + column;
	}

	else{
		return LCD_INVALID_PARAM;
	}

	return LCD_SendCommand(lcd, LCD_CMD_SET_DDRAM_ADDR|address);
}


LCD_Status_t LCD_WriteChar(LCD_Handle_t *lcd, char data){

	if(lcd == NULL){
		return LCD_INVALID_PARAM;
	}

	return LCD_SendData(lcd, (uint8_t)data);
}

LCD_Status_t LCD_WriteString(LCD_Handle_t *lcd, const char *str){

	LCD_Status_t status;

	if(lcd == NULL || str== NULL){
		return LCD_INVALID_PARAM;
	}

	while(*str != '\0'){
		status=LCD_WriteChar(lcd, *str);

		if(status != LCD_OK){
			return status;
		}
		str++;
	}
	return LCD_OK;
}

LCD_Status_t LCD_CreateChar(LCD_Handle_t *lcd, uint8_t location, const uint8_t pattern[8]){

	LCD_Status_t status;

	if((lcd == NULL) || (pattern == NULL) || (location >7U)){
		return LCD_INVALID_PARAM;
	}

	status = LCD_SendCommand(lcd, (LCD_CMD_SET_CGRAM_ADDR|(location << 3)));

	if(status != LCD_OK){
		return status;
	}

	for (uint8_t i = 0U; i < 8U ; i++) {

		status = LCD_SendData(lcd, pattern[i]);

		if(status != LCD_OK){
			return status;
		}
	}
	return LCD_SendCommand(lcd, LCD_CMD_SET_DDRAM_ADDR);
}

static LCD_Status_t LCD_GPIO_InitSequence(LCD_Handle_t *lcd)
{

	LCD_GPIO_EN_Low();
	LCD_GPIO_RS_Low();
	LCD_GPIO_ClearDataLines();
    HAL_Delay(LCD1602_POWER_ON_DELAY_MS);

    LCD_GPIO_WriteNibble(0x03U);
    HAL_Delay(5U);

    LCD_GPIO_WriteNibble(0x03U);
    HAL_Delay(1U);

    LCD_GPIO_WriteNibble(0x03U);
    HAL_Delay(1U);

    if(lcd->mode == LCD_MODE_4BIT){
    LCD_GPIO_WriteNibble(0x02U);
    HAL_Delay(1U);
    }

    return LCD_OK;
}

static LCD_Status_t LCD_I2C_InitSequence(LCD_Handle_t *lcd)
{
    LCD_Status_t status;

    HAL_Delay(LCD1602_POWER_ON_DELAY_MS);

    /* Force LCD into 4-bit mode */
    status = LCD_I2C_WriteNibble(lcd, 0x03U, 0U);
    if (status != LCD_OK){
        return status;
    }
    HAL_Delay(5U);

    status = LCD_I2C_WriteNibble(lcd, 0x03U, 0U);
    if (status != LCD_OK){
        return status;
    }
    HAL_Delay(1U);

    status = LCD_I2C_WriteNibble(lcd, 0x03U, 0U);
    if (status != LCD_OK)    {
        return status;
    }
    HAL_Delay(1U);

    status = LCD_I2C_WriteNibble(lcd, 0x02U, 0U);
    if (status != LCD_OK)   {
        return status;
    }
    HAL_Delay(1U);

    return LCD_OK;
}


LCD_Status_t LCD_Init(LCD_Handle_t *lcd){

	LCD_Status_t status;
	uint8_t command;
	if(lcd == NULL){
		return LCD_INVALID_PARAM;
	}


	if (lcd->interface == LCD_INTERFACE_GPIO)
	    {
	        status = LCD_GPIO_InitSequence(lcd);
	    }
	    else if (lcd->interface == LCD_INTERFACE_I2C && lcd->mode == LCD_MODE_4BIT)
	    {
	    	 if (lcd->hi2c == NULL) {
	    	            return LCD_INVALID_PARAM;
	    	        }
	        status = LCD_I2C_InitSequence(lcd);

	    }
	    else
	    {
	        return LCD_INVALID_PARAM;
	    }

	if(status != LCD_OK){
	    return status;
	}

    command = LCD_CMD_FUNCTION_SET;

    if (lcd->mode == LCD_MODE_8BIT){
        command |= 0x10U;
    }

    if (lcd->lines == LCD_LINES_2){
        command |= 0x08U;
    }

    if (lcd->font == LCD_FONT_5x10){
        command |= 0x04U;
    }

    status = LCD_SendCommand(lcd, command);

    if (status != LCD_OK){
        return status;
    }

    status = LCD_ApplyDisplayConfig(lcd);

    if (status != LCD_OK){
        return status;
    }

    status = LCD_Clear(lcd);

      if (status != LCD_OK) {
          return status;
      }

      status = LCD_SendCommand(lcd, LCD_CMD_ENTRY_MODE_SET | 0x02U);

	  return status;
}

