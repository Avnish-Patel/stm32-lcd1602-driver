/*
 * lcd1602_port.h
 *
 *  Created on: Aug 8, 2026
 *      Author: dev
 */

#ifndef INC_LCD1602_PORT_H_
#define INC_LCD1602_PORT_H_

#include "main.h"


#define LCD1602_RS_PORT    GPIOE
#define LCD1602_RS_PIN     GPIO_PIN_0
#define LCD1602_RS_BIT     0U

#define LCD1602_EN_PORT    GPIOE
#define LCD1602_EN_PIN     GPIO_PIN_1
#define LCD1602_EN_BIT     2U

#define LCD1602_D0_PORT    GPIOE
#define LCD1602_D0_PIN     GPIO_PIN_2

#define LCD1602_D1_PORT    GPIOE
#define LCD1602_D1_PIN     GPIO_PIN_3

#define LCD1602_D2_PORT    GPIOE
#define LCD1602_D2_PIN     GPIO_PIN_4

#define LCD1602_D3_PORT    GPIOE
#define LCD1602_D3_PIN     GPIO_PIN_5

#define LCD1602_D4_PORT    GPIOE
#define LCD1602_D4_PIN     GPIO_PIN_6
#define LCD1602_D4_BIT     4U

#define LCD1602_D5_PORT    GPIOE
#define LCD1602_D5_PIN     GPIO_PIN_7
#define LCD1602_D5_BIT     5U

#define LCD1602_D6_PORT    GPIOE
#define LCD1602_D6_PIN     GPIO_PIN_8
#define LCD1602_D6_BIT     6U

#define LCD1602_D7_PORT    GPIOE
#define LCD1602_D7_PIN     GPIO_PIN_9
#define LCD1602_D7_BIT     7U

#define LCD1602_RW_PORT    GPIOE
#define LCD1602_RW_PIN     GPIO_PIN_10
#define LCD1602_RW_BIT     1U


#define LCD1602_BL_BIT        3U


#endif /* INC_LCD1602_PORT_H_ */
