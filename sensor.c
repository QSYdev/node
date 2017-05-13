#include "c_types.h"
#include "osapi.h"
#include "eagle_soc.h"
#include "gpio.h"
#include "ets_sys.h"
#include "sensor.h"

static void gpio_handler();

void ICACHE_FLASH_ATTR sensor_init()
{
}

void ICACHE_FLASH_ATTR sensor_arm()
{
	ETS_GPIO_INTR_DISABLE();

	gpio_pin_intr_state_set(GPIO_ID_PIN(0), GPIO_PIN_INTR_POSEDGE);
	ETS_GPIO_INTR_ATTACH(gpio_handler, NULL);

	ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR sensor_disarm()
{
	ETS_GPIO_INTR_DISABLE();

	gpio_pin_intr_state_set(GPIO_ID_PIN(0), GPIO_PIN_INTR_DISABLE);

	ETS_GPIO_INTR_ENABLE();
}

static void gpio_handler()
{
	os_printf("Button pressed!\n");

	/* Clear interrupt */
	uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
}
