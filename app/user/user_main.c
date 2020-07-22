#include "user_main.h"

uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;
        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;
        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;
        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}

void user_rf_pre_init(void) {
}

void user_init(void) {

	uart_init(BIT_RATE_74880, BIT_RATE_74880);
	wifi_set_opmode(NULL_MODE);


	ssd1306GPIOSetup();
	ssd1306Reset();

	ssd1306PrepareBuffer();
	ssd1306Init();

	GuiDrawLine(0, 0, 128, 32, COLOR_WHITE);
	GuiDrawLine(128, 0, 0, 32, COLOR_WHITE);

	ssd1306Flush();

}



