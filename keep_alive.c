#include "keep_alive.h"
#include <stdbool.h>
#include "tcp_connection.h"
#include "packet.h"
#include "udp_connection.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "espmissingincludes.h"
#include "network.h"

#define PERIOD 500

static struct qsy_packet keepalive_packet;

static os_timer_t start_timer;
static void keep_alive_init(void *arg);

static os_timer_t msg_timer;
static void timer_cb(void *arg);

void ICACHE_FLASH_ATTR keep_alive_start(void)
{
	uint32_t delay = system_get_time() % PERIOD;
	if (!delay)
		delay = 10;
	os_timer_disarm(&start_timer);
	os_timer_setfn(&start_timer, (os_timer_func_t *) keep_alive_init, NULL);
	os_timer_arm(&start_timer, delay, false);
	os_printf("Delay = %d\n", delay);
}

static void ICACHE_FLASH_ATTR keep_alive_init(void *arg)
{
	os_timer_disarm(&start_timer);

	packet_init(&keepalive_packet);
	packet_set_type(&keepalive_packet, keepalive);

	os_timer_disarm(&msg_timer);
	os_timer_setfn(&msg_timer, (os_timer_func_t *) timer_cb, NULL);
	os_timer_arm(&msg_timer, PERIOD, true);
	os_printf("Keepalive started\n");
}

void ICACHE_FLASH_ATTR keep_alive_stop(void)
{
	os_timer_disarm(&start_timer);
	os_timer_disarm(&msg_timer);
	os_printf("Keepalive stopped\n");
}

static void timer_cb(void *arg)
{
	tcp_connection_send_packet(&keepalive_packet);
}
