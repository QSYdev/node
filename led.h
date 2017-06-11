#ifndef LED_H
#define LED_H

#include <stdint.h>
void ICACHE_FLASH_ATTR led_init(void);
void ICACHE_FLASH_ATTR led_set_color(uint16_t color);
 
#endif	/*  */
