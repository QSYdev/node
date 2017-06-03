#include "keep_alive.h"
#include "tcp_connection.h"
#include "message.h"
#include "udp_connection.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"

#define PERIOD 500

static struct qsy_message keep_alive_msg =
{
	.signature = {'Q', 'S', 'Y'},
	.type = KEEP_ALIVE_MSG
};

static volatile os_timer_t msg_timer;
static void timer_cb(void* arg);

void ICACHE_FLASH_ATTR keep_alive_start(void) {
    os_timer_disarm(&msg_timer);
    os_timer_setfn(&msg_timer, (os_timer_func_t*) timer_cb, NULL);
    os_timer_arm(&msg_timer, PERIOD, true);
}

void ICACHE_FLASH_ATTR keep_alive_stop(void) {
    os_timer_disarm(&msg_timer);
}

static void timer_cb(void* arg) {
    tcp_connection_send_message((void*) &keep_alive_msg, QSY_MSG_SIZE);
}
