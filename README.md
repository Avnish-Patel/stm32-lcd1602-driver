# STM32 LCD1602 Driver

A reusable **LCD1602 / HD44780-compatible character LCD driver for STM32** using the STM32 HAL library.

The driver supports both **direct GPIO** and **I2C PCF8574 backpack** interfaces, with configurable 4-bit and 8-bit GPIO modes.

## Features

* STM32 HAL-based driver
* LCD1602 / HD44780-compatible displays
* GPIO interface:

  * 4-bit mode
  * 8-bit mode
* I2C interface:

  * 4-bit LCD operation
  * PCF8574 I2C backpack
* Display ON/OFF control
* Cursor ON/OFF control
* Cursor blinking
* Cursor positioning
* Display shifting
* Cursor shifting
* Clear display
* Return home
* Character and string writing
* Custom character generation using CGRAM
* Configurable:

  * Number of lines
  * Character font
  * Display state
  * Cursor state
  * Blink state
  * Backlight state
  * I2C address
* Status/error return values for driver operations

## Supported Interfaces

### GPIO

The driver can operate the LCD directly through STM32 GPIO pins.

#### 4-bit mode

Uses:

```text
RS
EN
D4
D5
D6
D7
```

#### 8-bit mode

Uses:

```text
RS
EN
D0
D1
D2
D3
D4
D5
D6
D7
```

### I2C

The I2C interface uses a **PCF8574 I/O expander** commonly found on LCD1602 I2C backpacks.

The LCD remains in 4-bit mode while the PCF8574 controls the LCD control and data lines.

Typical backpack address:

```text
0x27
```

Other common addresses such as `0x3F` may also be used depending on the PCF8574 board.

## Project Structure

```text
stm32-lcd1602-driver/
│
├── lcd1602.c
├── lcd1602.h
├── lcd1602_port.h
└── README.md
```

### `lcd1602.c`

Contains the main driver implementation, including:

* GPIO communication
* I2C communication
* LCD initialization
* Command handling
* Data transmission
* Cursor/display control
* Custom character generation

### `lcd1602.h`

Contains the public driver API, configuration types, status definitions and LCD commands.

### `lcd1602_port.h`

Contains the hardware-specific port configuration such as:

* GPIO ports
* GPIO pins
* I2C configuration
* PCF8574 bit mapping
* LCD timing configuration

## Example Usage

After configuring the required GPIO/I2C peripherals using STM32CubeMX, create and configure an LCD handle.

Example:

```c
LCD_Handle_t lcd = {
    .interface = LCD_INTERFACE_I2C,
    .mode = LCD_MODE_4BIT,
    .lines = LCD_LINES_2,
    .font = LCD_FONT_5x8,
    .display = LCD_DISPLAY_ON,
    .cursor = LCD_CURSOR_OFF,
    .blink = LCD_BLINK_OFF,
    .backLight = LCD_BACKLIGHT_ON,
    .hi2c = &hi2c1,
    .i2c_address = 0x27,
    .columns = 16
};
```

Initialize the LCD:

```c
LCD_Init(&lcd);
```

Write text:

```c
LCD_SetCursor(&lcd, 0, 0);
LCD_WriteString(&lcd, "Hello STM32");

LCD_SetCursor(&lcd, 1, 0);
LCD_WriteString(&lcd, "LCD1602 Driver");
```

Control the display:

```c
LCD_DisplayOn(&lcd);
LCD_CursorOn(&lcd);
LCD_BlinkOn(&lcd);
```

## Custom Characters

The driver supports creating up to **8 custom characters** using the LCD CGRAM.

Example:

```c
uint8_t heart[8] = {
    0x00,
    0x0A,
    0x1F,
    0x1F,
    0x1F,
    0x0E,
    0x04,
    0x00
};

LCD_CreateChar(&lcd, 0, heart);
```

The custom character can then be displayed using:

```c
LCD_WriteChar(&lcd, 0);
```

## Hardware Requirements

* STM32 development board
* LCD1602 display
* Jumper wires
* Breadboard
* For I2C operation:

  * PCF8574 LCD I2C backpack

## Software Requirements

* STM32CubeIDE
* STM32 HAL
* STM32CubeMX-generated project
* C compiler supporting STM32 HAL

## Notes

The driver is designed as a reusable module rather than being tied to a single STM32 project.

Hardware-specific definitions should be kept in `lcd1602_port.h`, allowing the main driver implementation to remain independent of the selected STM32 pins and peripherals.

For I2C communication, STM32 HAL expects the 7-bit device address shifted left by one bit. The driver therefore performs:

```c
lcd->i2c_address << 1
```

before calling `HAL_I2C_Master_Transmit()`.

## Current Status

**Working driver**

Implemented and tested functionality includes:

* GPIO 4-bit communication
* GPIO 8-bit communication
* I2C 4-bit communication
* LCD initialization
* Character/string output
* Cursor positioning
* Display/cursor/blink control
* Display and cursor shifting
* Clear/home commands
* Custom characters

## Author

**Avnish Patel**

Embedded Systems / Firmware Development

---

This project was developed as part of hands-on STM32 embedded driver development and is intended for learning, experimentation, and reuse in STM32 projects.
