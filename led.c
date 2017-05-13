#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "ets_sys.h"
#include "eagle_soc.h"
#include "led.h"
#include "gpio.h"



#define GPIO2 4

static void led_toggle();

static uint16_t color;
static uint16_t toggle_color;

static volatile os_timer_t blink_timer;

void ICACHE_FLASH_ATTR led_init()
{
	gpio_init();
	gpio_output_set(0, 0 , GPIO2, 0);
	gpio_output_set(GPIO2, 0, 0, 0);
}

void ICACHE_FLASH_ATTR led_set(uint16_t col)
{
	color = col;
	if(color)
		gpio_output_set(0, GPIO2, 0, 0); 
	else
		gpio_output_set(GPIO2, 0, 0, 0);     
}

void ICACHE_FLASH_ATTR led_blink_start(uint16_t color, uint16_t interval)
{
	toggle_color = color;
	os_timer_disarm(&blink_timer);
	os_timer_setfn(&blink_timer, (os_timer_func_t*) led_toggle, NULL);
	os_timer_arm(&blink_timer, interval, true);
}

void ICACHE_FLASH_ATTR led_blink_stop()
{
	os_timer_disarm(&blink_timer);
	led_set(0);
}

static void ICACHE_FLASH_ATTR led_toggle()
{
	if (color)
		led_set(0);
	else
		led_set(toggle_color);
}