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
#include "queue.h"

static struct _esp_tcp tcp_params = {
	.local_port = QSY_LISTEN_PORT
};

static struct espconn listen_socket = {
	.type = ESPCONN_TCP,
	.state = ESPCONN_NONE,
	.proto = &tcp_params
};

static void ICACHE_FLASH_ATTR do_send_message(struct qsy_message *message);
static void ICACHE_FLASH_ATTR connect_cb(void *arg);
static void ICACHE_FLASH_ATTR reconnect_cb(void *arg, int8_t error);
static void ICACHE_FLASH_ATTR disconnect_cb(void *arg);
static void recv_cb(void *conn, char *pdata, unsigned short len);
static void sent_cb(void *conn);

static struct espconn *connection;
static struct queue *message_queue;
static volatile bool ready_to_send;

/* Not idempotent! */
void ICACHE_FLASH_ATTR tcp_connection_init(void)
{
	message_queue = queue_create(QSY_MSG_SIZE, 4);

	if (!message_queue)
		os_printf("Could not create message queue!");

	espconn_regist_connectcb(&listen_socket,
				 (espconn_connect_callback) connect_cb);
	espconn_regist_reconcb(&listen_socket,
			       (espconn_reconnect_callback) reconnect_cb);

	espconn_accept(&listen_socket);

	if (espconn_regist_time(&listen_socket, 0, 0)) {
		os_printf("tcp_connection: failed to set timeout.\n");
	}

	ready_to_send = true;
}

static void ICACHE_FLASH_ATTR connect_cb(void *arg)
{
	connection = (struct espconn *) arg;
	os_printf("Connected to terminal.\n");

	if (espconn_set_opt
	    (&listen_socket, ESPCONN_KEEPALIVE | ESPCONN_NODELAY)) {
		os_printf("tcp_connection: failed to set options.\n");
	}

	if (espconn_regist_sentcb
	    (connection, (espconn_sent_callback) sent_cb)) {
		os_printf
		    ("tcp_connection: failed to set receive callback.\n");
	}

	if (espconn_regist_recvcb
	    (connection, (espconn_recv_callback) recv_cb)) {
		os_printf
		    ("tcp_connection: failed to set receive callback.\n");
	}

	if (espconn_regist_disconcb
	    (connection, (espconn_connect_callback) disconnect_cb)) {
		os_printf
		    ("tcp_connection: failed to set disconnect callback.\n");
	}

	node_notify(GOT_TERMINAL);
}

static void ICACHE_FLASH_ATTR reconnect_cb(void *arg, int8_t error)
{
	switch (error) {
	case ESPCONN_TIMEOUT:
		os_printf("TCP connection timed out.\n");
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

static void ICACHE_FLASH_ATTR recv_cb(void *conn, char *pdata, unsigned short len)
{
	message_receiver_cb(pdata, len);
}

static void ICACHE_FLASH_ATTR do_send_message(struct qsy_message *message)
{
	int8_t res;
	if (res = espconn_send(connection, message, QSY_MSG_SIZE)) {
		os_printf("tcp_connection_send_message: \n");
		switch (res) {
		case ESPCONN_MEM:
			os_printf("Out of memory.\n");
			break;
		case ESPCONN_ARG:
			os_printf("Illegal argument.\n");
			break;
		case ESPCONN_MAX:
			os_printf("Outgoing buffer full.\n");
			break;
		case ESPCONN_IF:
			os_printf("UDP failed. (?)\n");
			break;
		default:
			os_printf("Returned %d\n", (int) res);
		}
	}
}

static struct qsy_message msg;

static void ICACHE_FLASH_ATTR sent_cb(void *conn)
{
	if (!queue_is_empty(message_queue)) {
		
		queue_pop(message_queue, &msg);
		do_send_message(&msg);
	} else {
		ready_to_send = true;
	}
}

void ICACHE_FLASH_ATTR tcp_connection_send_message(struct qsy_message *message)
{
	if (ready_to_send) {
		msg = *message;
		do_send_message(&msg);
		ready_to_send = false;
	} else {
		if (queue_is_full(message_queue))
			os_printf("Message queue full. Dropping message!\n");
		else
			queue_push(message_queue, message);
	}

}
