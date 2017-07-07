#include "hello.h"
#include "udp_connection.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "message.h"
#include "protocol.h"
#include "led.h"
#include "network.h"
#include "espmissingincludes.h"

#define PERIOD 500

#ifndef NODE_ID
#error ID for this node not defined!
#endif

static struct qsy_message hello_msg = {
	.signature = {'Q', 'S', 'Y'},
	.type = HELLO_MSG,
	.id = NODE_ID
};

static os_timer_t msg_timer;
static void timer_cb(void *arg);
static uint16_t color;

void ICACHE_FLASH_ATTR hello_start(void)
{
	udp_connection_init();

	color = 0;
	led_set_color(color);

	os_timer_disarm(&msg_timer);
	os_timer_setfn(&msg_timer, (os_timer_func_t *) timer_cb, NULL);
	os_timer_arm(&msg_timer, PERIOD, true);
}

void ICACHE_FLASH_ATTR hello_stop(void)
{
	udp_connection_stop();
	os_timer_disarm(&msg_timer);
	led_set_color(color = 0);
}

static void timer_cb(void *arg)
{
	hello_msg.id = htons(NODE_ID);
	udp_connection_send_message((void *) &hello_msg, QSY_MSG_SIZE);
	if (color == 0) {
		color = 0xF000;
	} else {
		color = 0;
	}
	led_set_color(color);
}
