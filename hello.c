#include "hello.h"
#include "udp_connection.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "packet.h"
#include "protocol.h"
#include "led.h"
#include "network.h"
#include "espmissingincludes.h"

#define PERIOD 500

#ifndef NODE_ID
#error ID for this node not defined!
#endif

static struct qsy_packet hello_packet;

static os_timer_t msg_timer;
static void timer_cb(void *arg);
static struct color color;

void ICACHE_FLASH_ATTR hello_start(void)
{
	udp_connection_init();

	packet_init(&hello_packet);
	packet_set_type(&hello_packet, hello);

	led_set_color(color);

	os_timer_disarm(&msg_timer);
	os_timer_setfn(&msg_timer, (os_timer_func_t *) timer_cb, NULL);
	os_timer_arm(&msg_timer, PERIOD, true);
}

void ICACHE_FLASH_ATTR hello_stop(void)
{
	udp_connection_stop();
	os_timer_disarm(&msg_timer);
	color.red = color.green = color.blue = 0;
	led_set_color(color);
}

static void timer_cb(void *arg)
{
	udp_connection_send_message(&hello_packet, QSY_PACKET_SIZE);
	if (color.red || color.green || color.blue) {
		color.blue = 0xF;
	} else {
		color.red = color.green = color.blue = 0;
	}
	led_set_color(color);
}
