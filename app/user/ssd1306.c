/*
 * ssd1306.c
 *  Created on: Jul 21, 2020
 *      Author: Yanye
 */

#include "ssd1306.h"

static uint8_t *DISPLAY_BUFFER;

static const uint32_t INIT_CMD_LEN = 22;
static const uint8_t INIT_COMMANDS[] = {CMD_DISPLAY_OFF, CMD_DISPLAY_CLK_DIV, 0x80, 0xA8, 0x1F, CMD_DISPLAY_OFFSET,
		0x00, 0x40, 0x8D, 0x14, 0xA0, 0xC0, 0xDA, 0x02, CMD_CONTRAST, 0x00, 0xD9, 0xF1, 0xDB, 0x64, 0xA4, 0xA6};

static void spiSendByte(uint8_t byte);
static void ssd1306SendCmd(uint8_t cmd);
static void ssd1306SendData(uint8_t data);

BOOL ICACHE_FLASH_ATTR ssd1306PrepareBuffer() {
	if(DISPLAY_BUFFER == NULL) {
		DISPLAY_BUFFER = (uint8_t *)os_malloc(sizeof(uint8_t) * (DISPLAY_WIDTH >> 3) * DISPLAY_HEIGHT);
		return TRUE;
	}
	return FALSE;
}

void ICACHE_FLASH_ATTR ssd1306GPIOSetup() {
	// RESET_PIN
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);

	// DATACMD_PIN
	// default pull down, ssd1306 is in command mode.
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U);

	// MOSI
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTMS_U);

	// SCLK
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);

	// CS
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTDO_U);
	GPIO_OUTPUT_SET(SPI_CS, GPIO_PIN_HIGH);
}

/**
 * @brief set RES# pin low at least 3us to reset display controller.
 * */
void ICACHE_FLASH_ATTR ssd1306Reset() {
	GPIO_OUTPUT_SET(RESET_PIN_NUM, GPIO_PIN_LOW);
	os_delay_us(10);
	GPIO_OUTPUT_SET(RESET_PIN_NUM, GPIO_PIN_HIGH);
}

/**
 * @brief ssd1306 init progress, send initialization frame to display controller ssd1306.
 * */
void ICACHE_FLASH_ATTR ssd1306Init() {
	uint32_t i = 0, j = 0;
	// 512 == (DISPLAY_WIDTH >> 3) * DISPLAY_HEIGHT
	os_memset(DISPLAY_BUFFER, 0x00, sizeof(uint8_t) * 512);
	// send init commands
	for(; i < INIT_CMD_LEN; i++) {
		ssd1306SendCmd(*(INIT_COMMANDS + i));
	}
	// clear oled display data;
	for(i = 0; i < 4; i++) {
		ssd1306SendCmd(0xB0 + i);
		for(j = 0; j < 128; j++) {
			ssd1306SendData(0x00);
		}
	}
	// turn on display
	ssd1306SendCmd(0xAF);
}



/**
 * @brief turn on/off a pxiel on screen
 * @param x x axis position
 * @param y x axis position
 * @param color only 'COLOR_WHITE' or 'COLOR_BLACK'
 * */
void ssd1306SetPixel(uint32_t x, uint32_t y, uint8_t color) {
	if(color) {
		*(DISPLAY_BUFFER + (y << 4) + (x >> 3)) |= (0x1 << (x % 8));
	}else {
		*(DISPLAY_BUFFER + (y << 4) + (x >> 3)) &= ~(0x1 << (x % 8));
	}
}

/**
 * @brief This command sets the Contrast Setting of the display.
 * @param constrast contrast value from 0x00 to 0xFF, 0x00 is minimize and 0xFF is maximize.
 * */
void ssd1306SetContrast(uint8_t constrast) {
	ssd1306SendCmd(CMD_CONTRAST);
	ssd1306SendCmd(constrast);
}

/**
 * @brief turn on display
 * */
void ssd1306DisplayOn() {
	ssd1306SendCmd(CMD_DISPLAY_ON);
}

/**
 * @brief turn off display into sleep mode, but not clear GDDRAM
 * */
void ssd1306DisplayOff() {
	ssd1306SendCmd(CMD_DISPLAY_OFF);
}

/**
 * @brief flush display buffer to ssd1306 GDDRAM
 * */
void ssd1306Flush() {
	uint32_t page, column, i;
	uint8_t data, byte;
	for(page = 0; page < DISPLAY_PAGE; page++) {
		ssd1306SendCmd(0xB0 + page);
		for(column = 0; column < DISPLAY_WIDTH; column++) {
			// cat data
			for(i = 0, data = 0x00; i < 8; i++) {
				byte = *(DISPLAY_BUFFER + (page * 128) + (i << 4) + (column >> 3));
				data |= (((byte >> (column % 8)) & 0x1) << i);
			}
			ssd1306SendData(data);
		}
	}
}

/**
 * @brief soft spi send one byte, MSB first.
 * @param byte (0x00~0xFF)
 * */
static void spiSendByte(uint8_t byte) {
	GPIO_OUTPUT_SET(SPI_CS, GPIO_PIN_LOW);
	int i = 7;
	for(; i >= 0; i--) {
		GPIO_OUTPUT_SET(SPI_SCLK, GPIO_PIN_LOW);
		if((byte >> i) & 0x1) {
			GPIO_OUTPUT_SET(SPI_MOSI, GPIO_PIN_HIGH);
		}else {
			GPIO_OUTPUT_SET(SPI_MOSI, GPIO_PIN_LOW);
		}
		GPIO_OUTPUT_SET(SPI_SCLK, GPIO_PIN_HIGH);
	}
	GPIO_OUTPUT_SET(SPI_CS, GPIO_PIN_HIGH);
}

/**
 * @brief send command to ssd1306, must pull down 'DATACMD_PIN_NUM' first.
 * @param cmd command which send to ssd1306.
 * */
static void ssd1306SendCmd(uint8_t cmd) {
	GPIO_OUTPUT_SET(DATACMD_PIN_NUM, TRANSFER_COMMAND_MODE);
	spiSendByte(cmd);
}

/**
 * @brief send display data to ssd1306, must pull up 'DATACMD_PIN_NUM' first.
 * @param data display data.
 * */
static void ssd1306SendData(uint8_t data) {
	GPIO_OUTPUT_SET(DATACMD_PIN_NUM, TRANSFER_DATA_MODE);
	spiSendByte(data);
}

/**
 * @brief release display buffer in application layer.
 * @return TRUE: success, FALSE: DISPLAY_BUFFER is null pointer.
 * */
BOOL ICACHE_FLASH_ATTR ssd1306DestoryBuffer() {
	if(DISPLAY_BUFFER != NULL) {
		os_free(DISPLAY_BUFFER);
		return TRUE;
	}
	return FALSE;
}
