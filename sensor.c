#include "c_types.h"
#include "osapi.h"
#include <stdbool.h>
#include "eagle_soc.h"
#include "gpio.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "sensor.h"
#include "command.h"
#include "espmissingincludes.h"

/* en milisegundos */
#define READ_PERIOD 50
#define FAR_THRESHOLD 512
#define CLOSE_THRESHOLD 128

os_timer_t timer;

static void timer_cb(void *arg);

void ICACHE_FLASH_ATTR sensor_init(void)
{
}

void ICACHE_FLASH_ATTR sensor_start(void)
{
	os_timer_disarm(&timer);
	os_timer_setfn(&timer, timer_cb, NULL);
	os_timer_arm(&timer, READ_PERIOD, true);
}

void ICACHE_FLASH_ATTR sensor_stop(void)
{
	os_timer_disarm(&timer);
}

static void ICACHE_FLASH_ATTR timer_cb(void *arg)
{
	uint16_t adc = system_adc_read();
	if (adc > CLOSE_THRESHOLD)
		command_touched();
	else if (adc > FAR_THRESHOLD)
		command_touched();
	os_printf("%u\n", adc);
}
