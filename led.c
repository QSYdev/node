#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "ets_sys.h"
#include "eagle_soc.h"
#include "led.h"
#include "gpio.h"
#include "espmissingincludes.h"

#define GPIO2 4

void ICACHE_FLASH_ATTR led_init(void)
{
	gpio_init();
	gpio_output_set(0, 0, GPIO2, 0);
	gpio_output_set(GPIO2, 0, 0, 0);
}

void ICACHE_FLASH_ATTR led_set_color(struct color col)
{
	if (col.red || col.green || col.blue) {
		gpio_output_set(0, GPIO2, 0, 0);
	} else {
		gpio_output_set(GPIO2, 0, 0, 0);
	}
}

void ICACHE_FLASH_ATTR led_turn_off()
{
	struct color color;
	color.red = color.green = color.blue = 0;
	led_set_color(color);
}
