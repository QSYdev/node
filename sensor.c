#include "c_types.h"
#include "osapi.h"
#include "eagle_soc.h"
#include "gpio.h"
#include "ets_sys.h"
#include "sensor.h"
#include "command.h"

static void sensor_cb();

void ICACHE_FLASH_ATTR sensor_init(void)
{
	ETS_GPIO_INTR_DISABLE();

	gpio_pin_intr_state_set(GPIO_ID_PIN(0), GPIO_PIN_INTR_POSEDGE);
	ETS_GPIO_INTR_ATTACH(sensor_cb, NULL);

	ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR sensor_stop(void)
{
	ETS_GPIO_INTR_DISABLE();

	gpio_pin_intr_state_set(GPIO_ID_PIN(0), GPIO_PIN_INTR_DISABLE);

	ETS_GPIO_INTR_ENABLE();
}

static void sensor_cb(void)
{
	command_touched();
	uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
}
