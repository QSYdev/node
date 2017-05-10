#include <stdbool.h>
#include "ets_sys.h"
#include "led.h"
#include "gpio.h"
#include "eagle_soc.h"

#define GPIO2 4

void ICACHE_FLASH_ATTR led_init()
{
	gpio_init();
        gpio_output_set(0, 0 , GPIO2, 0);
        gpio_output_set(GPIO2, 0, 0, 0);
}

void ICACHE_FLASH_ATTR led_set(uint16_t color)
{
	if(color)
		gpio_output_set(0, GPIO2, 0, 0); 
	else
		gpio_output_set(GPIO2, 0, 0, 0);     
}