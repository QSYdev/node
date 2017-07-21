#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "ets_sys.h"
#include "eagle_soc.h"
#include "led.h"
#include "gpio.h"
#include "espmissingincludes.h"

#define GPIO2 4

static struct color color;
static bool on;
static os_timer_t blink_timer;

static void timer_cb(void *arg);

void ICACHE_FLASH_ATTR led_init(void)
{
	color.blue = 0xF;
	gpio_output_set(0, 0, GPIO2, 0);
	led_turn_off();
}

void ICACHE_FLASH_ATTR led_set_color(struct color col)
{
	color = col;
	if (on) {
		led_turn_on();
	}
}

/* Configura y prende */
void ICACHE_FLASH_ATTR led_turn_on()
{
	/* TODO: setear el color según corresponda. */
	if (color.red || color.green || color.blue) {
		gpio_output_set(0, GPIO2, 0, 0);
	} else {
		gpio_output_set(GPIO2, 0, 0, 0);
	}
	on = true;
}

void ICACHE_FLASH_ATTR led_turn_off()
{
	gpio_output_set(GPIO2, 0, 0, 0);
	on = false;
}

void ICACHE_FLASH_ATTR led_set_blink(uint16_t period)
{
	if (period == 0) {
		os_timer_disarm(&blink_timer);
	} else {
		os_timer_setfn(&blink_timer, timer_cb, NULL);
		os_timer_arm(&blink_timer, period, true);
	}
}

static void timer_cb(void *arg)
{
	on ? led_turn_off() : led_turn_on();
}
