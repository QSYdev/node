#include <stdbool.h>
#include "command.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "network.h"
#include "protocol.h"
#include "message.h"
#include "led.h"
#include "node.h"

static struct _esp_tcp tcp_params =
{
	.local_port = QSY_LISTEN_PORT
};

static struct espconn listen_socket =
{
	.type = ESPCONN_TCP,
	.state = ESPCONN_NONE,
	.proto = &tcp_params
};

static volatile os_timer_t delay_timer;
static void command_function(void *parg);
static uint16_t color;

static void ICACHE_FLASH_ATTR listen();

/* Listening callbacks */
static void ICACHE_FLASH_ATTR connect_cb(void *arg);
static void ICACHE_FLASH_ATTR reconnect_cb(void *arg, int8_t error);
static void ICACHE_FLASH_ATTR disconnect_cb(void *arg);

/* Actual connection callbacks */
static void recv_callback(void * arg, char *pdata, unsigned short len);

void ICACHE_FLASH_ATTR command_init()
{
	os_printf("command_init:\n");
	listen();
}

static ICACHE_FLASH_ATTR void listen()
{
	espconn_regist_connectcb(&listen_socket,
	 (espconn_connect_callback) connect_cb);
	espconn_regist_reconcb(&listen_socket, (espconn_reconnect_callback) reconnect_cb);

	espconn_accept(&listen_socket);

	if(espconn_regist_time(&listen_socket, 0, 0))
		os_printf("command_init: failed to set timeout.\n");
}

static void ICACHE_FLASH_ATTR connect_cb(void *connection)
{
	os_printf("Connected to terminal.\n");
	if (espconn_set_opt(&listen_socket, ESPCONN_KEEPALIVE | ESPCONN_NODELAY))
		os_printf("connect_init: failed to set options.\n");
	if (espconn_regist_recvcb(connection, (espconn_recv_callback) recv_callback))
		os_printf("command_init: failed to set receive callback.\n");
	if (espconn_regist_disconcb(connection, (espconn_connect_callback) disconnect_cb))
		os_printf("command_init: failed to set disconnect callback.\n");
	node_notify(GOT_TERMINAL);
}

static void ICACHE_FLASH_ATTR reconnect_cb(void *arg, int8_t error)
{
	switch(error) {
	case ESPCONN_TIMEOUT:
		os_printf("Tcp connection timed out.\n");
		break;
	case ESPCONN_ABRT:
		os_printf("TCP connection aborted.\n");
		break;
	case ESPCONN_RST:
		os_printf("TCP connection reset.\n");
		break;
	case ESPCONN_CLSD:
		os_printf("TCP connection aborted.\n");
		break;
	case ESPCONN_CONN:
		os_printf("TCP connection failure.\n");
		break;
	}
	node_notify(TERMINAL_LOST);
}

static void ICACHE_FLASH_ATTR disconnect_cb(void *arg)
{
	os_printf("Terminal disconnected.\n");
	node_notify(TERMINAL_LOST);
}

static void recv_callback(void * connection, char *pdata, unsigned short len)
{	
	if(len != QSY_MSG_SIZE) {
		os_printf("Non-aligned messages are not yet implemented.");
	} else {
		struct qsy_message * msg = (struct qsy_message*) pdata;

		if (msg->signature[0] == 'Q' && msg->signature[1] == 'S' && msg->signature[2] == 'Y') {
			switch(msg->type) {
				case CMD_MSG: {
					os_timer_disarm(&delay_timer);
					//os_timer_setfn(&delay_timer, (os_timer_func_t*) command_function, (void *) ntohs(msg->color));
					color = msg->color;
					os_timer_setfn(&delay_timer, (os_timer_func_t*) command_function, NULL);
					os_timer_arm(&delay_timer, ntohl(msg->delay), false);
					break;
				}
			}

			/* Integral default promotion, beware */
			os_printf("Received TCP segment:\n");
			os_printf("  Command type: %u\n", msg->type);
			os_printf("  Phase: %u\n", msg->phase);
			os_printf("  Color: %u\n", msg->color);
			os_printf("  Delay: %u\n",  msg->delay);
			os_printf("  Id: %u\n",  msg->id);
		}

	}

}

static void ICACHE_FLASH_ATTR command_function(void *parg)
{
	//uint16_t color = (uint16_t *) parg;
	led_set(color);
	os_timer_disarm(&delay_timer);
}