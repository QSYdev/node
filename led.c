#include <stdbool.h>
#include "ets_sys.h"
#include "led.h"
#include "gpio.h"
#include "eagle_soc.h"

#define GPIO2 4

static uint16_t color;
static uint16_t last_color;

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

void ICACHE_FLASH_ATTR led_toggle()
{
        if(color) {
                last_color = color;
                color = 0;
        } else {
                if (!last_color)
                        last_color = 1;
                color = last_color;
        }
        
        led_set(color);
}