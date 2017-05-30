#include "command.h"
#include "message.h"
#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "network.h"
#include "protocol.h"
#include "led.h"

static volatile os_timer_t delay_timer;
static void ICACHE_FLASH_ATTR command_function(void *parg);

static unsigned char on;

void command_start(void) {
    on = 1;
}

void command_stop(void) {
    on = 0;
    os_timer_disarm(&delay_timer);
}

void command_message_received(char* pdata) {
    if (on) {
        struct qsy_message* msg = (struct qsy_message*) pdata;
        os_timer_disarm(&delay_timer);
        os_timer_setfn(&delay_timer, (os_timer_func_t*) command_function, (void *) ntohs(msg->color));
        os_timer_arm(&delay_timer, ntohl(msg->delay), false);
    }
}

static void ICACHE_FLASH_ATTR command_function(void *parg) {
	uint16_t color = (uint16_t *) parg;
    led_set_color(color);
	os_timer_disarm(&delay_timer);
}
