/*
 * ssd1306.h
 *
 *  Created on: Jul 21, 2020
 *      Author: Yanye
 */

#ifndef APP_USER_SSD1306_H_
#define APP_USER_SSD1306_H_

#include "c_types.h"
#include "os_type.h"
#include "gpio.h"
#include "mem.h"
#include "osapi.h"

#define GPIO_PIN_LOW    0
#define GPIO_PIN_HIGH   1

// reset pin active in low level
#define RESET_PIN_NUM      (5)
// low: command, heigh: display data
#define DATACMD_PIN_NUM    (4)

#define SPI_SCLK    14
#define SPI_MOSI    13
#define SPI_CS      15

#define TRANSFER_COMMAND_MODE    (GPIO_PIN_LOW)
#define TRANSFER_DATA_MODE    (GPIO_PIN_HIGH)

#define DISPLAY_WIDTH    128
#define DISPLAY_HEIGHT   32
#define DISPLAY_PAGE     4

#define COLOR_WHITE    (1)
#define COLOR_BLACK    (0)

#define CMD_CONTRAST       (0x81u)
#define CMD_DISPLAY_ON     (0xAFu)
#define CMD_DISPLAY_OFF    (0xAEu)

#define CMD_DISPLAY_OFFSET    (0xD3u)
#define CMD_DISPLAY_CLK_DIV   (0xD5u)


BOOL ICACHE_FLASH_ATTR ssd1306PrepareBuffer();

void ICACHE_FLASH_ATTR ssd1306GPIOSetup();

void ICACHE_FLASH_ATTR ssd1306Reset();

void ICACHE_FLASH_ATTR ssd1306Init();

void ssd1306SetPixel(uint32_t x, uint32_t y, uint8_t color);

void ssd1306Flush();

BOOL ICACHE_FLASH_ATTR ssd1306DestoryBuffer();



#endif /* APP_USER_SSD1306_H_ */
