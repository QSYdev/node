#include "command.h"
#include "message.h"
#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "network.h"
#include "protocol.h"
#include "led.h"

static volatile os_timer_t delay_timer;
static void ICACHE_FLASH_ATTR command_function(void *parg);

/* Módulo apagado o prendido */
static bool on = false;

/* Estamos esperando un touche */
static bool armed = false;

/* Valor de RTC al prender un LED */
static uint32_t rtc_value;

static struct qsy_message touche_message = {
	.signature = {'Q', 'S', 'Y'},
	.type = TOUCHE_MSG,
	/* Debe estar definida una macro "ID" con un uint16_t */
	.id = NODE_ID
};

void command_start(void)
{
	on = true;
}

void command_stop(void)
{
	on = false;
	armed = false;
	os_timer_disarm(&delay_timer);
}

void command_message_received(char *pdata)
{
	if (on) {
		struct qsy_message *msg = (struct qsy_message *) pdata;
		os_timer_disarm(&delay_timer);
		os_timer_setfn(&delay_timer, (os_timer_func_t *) command_function, (void *) ntohs(msg->color));
		os_timer_arm(&delay_timer, ntohl(msg->delay), false);
		os_printf("Command received color = %d delay = %ld.\n", ntohs(msg->color), ntohl(msg->delay));
	}
}

void command_touched(void)
{
	if (on && armed) {
		/* clocks */
		uint32_t delay = system_get_rtc_time() - rtc_value;
		/* microsegundos */
		delay *= system_rtc_clock_cali_proc() >> 12;
		/* milisegundos */
		delay /= 1000;
		touche_message.delay = htonl(delay);
		touche_message.id = htons(NODE_ID);
		tcp_connection_send_message((void *) &touche_message, QSY_MSG_SIZE);
		os_printf("Touche sent id = %d htonl(delay) = %ld.\nTouche sent delay = %ld\n", touche_message.id,touche_message.delay, delay);
		armed = false;
		led_set_color(0);
	}
}

static void ICACHE_FLASH_ATTR command_function(void *parg)
{
	uint16_t color = (uint16_t) parg;
	led_set_color(color);
	os_timer_disarm(&delay_timer);
	if (color) {
		armed = true;
		rtc_value = system_get_rtc_time();
	} else {
		armed = false;
	}
}
