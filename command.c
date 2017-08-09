#include "command.h"
#include "packet.h"
#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "network.h"
#include "protocol.h"
#include "led.h"
#include "espmissingincludes.h"
#include "tcp_connection.h"

static os_timer_t delay_timer;
static void ICACHE_FLASH_ATTR command_function(void *parg);

/* Módulo apagado o prendido */
static bool on = false;

/* Estamos esperando un touche */
static bool armed = false;

/* Valor de RTC al prender un LED */
static uint32_t rtc_value;

static struct color color;

static struct qsy_packet touche_packet;

void ICACHE_FLASH_ATTR command_start(void)
{
	packet_init(&touche_packet);
	packet_set_type(&touche_packet, touche);
	on = true;
}

void ICACHE_FLASH_ATTR command_stop(void)
{
	on = false;
	armed = false;
	os_timer_disarm(&delay_timer);
}

void ICACHE_FLASH_ATTR command_packet_received(char *pdata)
{
	if (on) {
		struct qsy_packet *packet = (struct qsy_packet *) pdata;
		os_timer_disarm(&delay_timer);
		color = packet_get_color(packet);
		if (packet_get_delay(packet)) {
			os_timer_setfn(&delay_timer, command_function, NULL);
			os_timer_arm(&delay_timer, packet_get_delay(packet),
			    	 false);
		} else {
			command_function(NULL);
		}
		os_printf("Command, color: %d\n", color);
	}
}

void ICACHE_FLASH_ATTR command_touched(void)
{
	if (on && armed) {
		uint32_t delay = system_get_time() - rtc_value;
		delay /= 1000;
		packet_set_delay(&touche_packet, delay);
		tcp_connection_send_packet(&touche_packet);
		armed = false;
		led_turn_off();
	}
}

static void ICACHE_FLASH_ATTR command_function(void *parg)
{
	led_set_color(color);
	led_turn_on();
	os_timer_disarm(&delay_timer);
	if (color.red || color.green || color.blue) {
		armed = true;
		rtc_value = system_get_time();
	} else {
		armed = false;
	}
}
