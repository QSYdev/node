#include "command.h"
#include "message.h"
#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "network.h"
#include "protocol.h"
#include "led.h"
#include "espmissingincludes.h"
#include "tcp_connection.h"

static  os_timer_t delay_timer;
static void ICACHE_FLASH_ATTR command_function(void *parg);

/* Módulo apagado o prendido */
static bool on = false;

/* Estamos esperando un touche */
static bool armed = false;

/* Valor de RTC al prender un LED */
static uint32_t rtc_value;

static uint16_t color;

static struct qsy_message touche_message = {
	.signature = {'Q', 'S', 'Y'},
	.type = TOUCHE_MSG,
	/* Debe estar definida una macro "ID" con un uint16_t */
	.id = NODE_ID
};

void ICACHE_FLASH_ATTR command_start(void)
{
	on = true;
}

void ICACHE_FLASH_ATTR command_stop(void)
{
	on = false;
	armed = false;
	os_timer_disarm(&delay_timer);
}

void ICACHE_FLASH_ATTR command_message_received(char *pdata)
{
	if (on) {
		struct qsy_message *msg = (struct qsy_message *) pdata;
		os_timer_disarm(&delay_timer);
		color = ntohs(msg->color);
		os_timer_setfn(&delay_timer, (os_timer_func_t *) command_function, NULL);
		os_timer_arm(&delay_timer, ntohl(msg->delay), false);
		os_printf("Command received color = %d delay = %u.\n", ntohs(msg->color), ntohl(msg->delay));
	}
}

void ICACHE_FLASH_ATTR command_touched(void)
{
	if (on && armed) {
		uint32_t delay = system_get_time() - rtc_value;
		delay /= 1000;
		touche_message.delay = htonl(delay);
		touche_message.id = htons(NODE_ID);
		tcp_connection_send_message(&touche_message);
		os_printf("Touche sent, delay = %u (htol: 0x%08X)\n", delay, touche_message.delay);
		armed = false;
		led_set_color(0);
	}
}

static void ICACHE_FLASH_ATTR command_function(void *parg)
{
	led_set_color(color);
	os_timer_disarm(&delay_timer);
	if (color) {
		armed = true;
		rtc_value = system_get_time();
	} else {
		armed = false;
	}
}
