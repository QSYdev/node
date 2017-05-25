#include "hello.h"
#include "udp_connection.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "discovery.h"
#include "message.h"
#include "protocol.h"

#define PERIOD 500

static struct qsy_message hello_msg =
{
	.signature={'Q','S','Y'},
	.type = HELLO_MSG
};

static volatile os_timer_t msg_timer;
static void timer_cb(void * arg);

void ICACHE_FLASH_ATTR hello_start(void) {
    udp_connection_init();

    os_timer_disarm(&msg_timer);
	os_timer_setfn(&msg_timer, (os_timer_func_t*) timer_cb, NULL);
	os_timer_arm(&msg_timer, PERIOD, true);
}

void ICACHE_FLASH_ATTR hello_stop(void) {
    udp_connection_stop();
    os_timer_disarm(&msg_timer);
}

static void timer_cb(void* arg) {
    udp_connection_send_message((void*) &hello_msg, QSY_MSG_SIZE);
}
