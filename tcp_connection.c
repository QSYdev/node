#include <stdbool.h>
#include "tcp_connection.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "protocol.h"
#include "message.h"
#include "node.h"
#include "message_receiver.h"

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

static void ICACHE_FLASH_ATTR connect_cb(void* arg);
static void ICACHE_FLASH_ATTR reconnect_cb(void* arg, int8_t error);
static void ICACHE_FLASH_ATTR disconnect_cb(void* arg);
static void recv_cb(void* conn, char* pdata, unsigned short len);

static struct espconn* connection;

void ICACHE_FLASH_ATTR tcp_connection_init(void) {
    espconn_regist_connectcb(&listen_socket,(espconn_connect_callback) connect_cb);
	espconn_regist_reconcb(&listen_socket, (espconn_reconnect_callback) reconnect_cb);

	espconn_accept(&listen_socket);

	if(espconn_regist_time(&listen_socket, 0, 0)) {
		os_printf("tcp_connection: failed to set timeout.\n");
    }

}

static void ICACHE_FLASH_ATTR connect_cb(void* arg) {
	connection = (struct espconn *)arg;
	os_printf("Connected to terminal.\n");

	if (espconn_set_opt(&listen_socket, ESPCONN_KEEPALIVE | ESPCONN_NODELAY)) {
		os_printf("tcp_connection: failed to set options.\n");
    }
	if (espconn_regist_recvcb(connection, (espconn_recv_callback) recv_cb)) {
		os_printf("tcp_connection: failed to set receive callback.\n");
    }
	if (espconn_regist_disconcb(connection, (espconn_connect_callback) disconnect_cb)) {
		os_printf("tcp_connection: failed to set disconnect callback.\n");
    }

	node_notify(GOT_TERMINAL);
}

static void ICACHE_FLASH_ATTR reconnect_cb(void* arg, int8_t error) {
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

static void ICACHE_FLASH_ATTR disconnect_cb(void* arg) {
	os_printf("Terminal disconnected.\n");
	node_notify(TERMINAL_LOST);
}

static void recv_cb(void* conn, char* pdata, unsigned short len) {
	message_receiver_cb(pdata, len);
}

void ICACHE_FLASH_ATTR tcp_connection_send_message(void* message, int length) {
	espconn_send(connection, message, length);
}
