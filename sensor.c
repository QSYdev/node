#include "c_types.h"
#include "osapi.h"
#include "eagle_soc.h"
#include "gpio.h"
#include "ets_sys.h"
#include "sensor.h"
#include "message.h"
#include  "tcp_connection.h"

static void sensor_cb();

static uint32_t rtc_value;

/* TODO: id? capaz deberiamos centralizar el armado de paquetes en un sólo módulo */
static struct qsy_message touche_message =
{
	.signature = {'Q','S','Y'}
};

void ICACHE_FLASH_ATTR sensor_init(void) {
}

void ICACHE_FLASH_ATTR sensor_start(void) {
	ETS_GPIO_INTR_DISABLE();

	gpio_pin_intr_state_set(GPIO_ID_PIN(0), GPIO_PIN_INTR_POSEDGE);
	ETS_GPIO_INTR_ATTACH(sensor_cb, NULL);

	ETS_GPIO_INTR_ENABLE();

	rtc_value = system_get_rtc_time();
}

void ICACHE_FLASH_ATTR sensor_disarm(void) {
	ETS_GPIO_INTR_DISABLE();

	gpio_pin_intr_state_set(GPIO_ID_PIN(0), GPIO_PIN_INTR_DISABLE);

	ETS_GPIO_INTR_ENABLE();
}

static void sensor_cb(void) {
	/* clocks*/
	uint32_t delay = rtc_value - system_get_rtc_time();
	/* microsegundos */
	delay *= system_rtc_clock_cali_proc() >> 12;
	/* milisegundos */
	delay /= 1000; 

	touche_message.delay = delay;
	tcp_connection_send_message(&touche_message, sizeof(touche_message));

	uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
}
