#include <stdbool.h>
#include <stdint.h>
#include "tcp_connection.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "protocol.h"
#include "packet.h"
#include "node.h"
#include "message_receiver.h"
#include "queue.h"
#include "espmissingincludes.h"

static struct _esp_tcp tcp_params = {
	.local_port = QSY_LISTEN_PORT
};

static struct espconn listen_socket = {
	.type = ESPCONN_TCP,
	.state = ESPCONN_NONE,
	.proto = &tcp_params
};

static void ICACHE_FLASH_ATTR do_send_packet(struct qsy_packet *packet);
static void ICACHE_FLASH_ATTR connect_cb(void *arg);
static void ICACHE_FLASH_ATTR reconnect_cb(void *arg, int8_t error);
static void ICACHE_FLASH_ATTR disconnect_cb(void *arg);
static void recv_cb(void *connection, char *pdata, unsigned short len);
static void sent_cb(void *connection);

static struct espconn *connection;
static struct queue *packet_queue;
static struct qsy_packet outgoing_packet;
static bool ready_to_send;
static bool tcp_up = false;

/* Not idempotent! */
void ICACHE_FLASH_ATTR tcp_connection_init(void)
{
	packet_queue = queue_create(QSY_PACKET_SIZE, 4);

	if (!packet_queue)
		os_printf("Could not create message queue!");

	espconn_regist_connectcb(&listen_socket,
				 (espconn_connect_callback) connect_cb);
	espconn_regist_reconcb(&listen_socket,
			       (espconn_reconnect_callback) reconnect_cb);

	espconn_accept(&listen_socket);

	if (espconn_regist_time(&listen_socket, 0, 0)) {
		os_printf("tcp_connection: failed to set timeout.\n");
	}
}

static void ICACHE_FLASH_ATTR connect_cb(void *arg)
{
	connection = (struct espconn *) arg;
	os_printf("Connected to terminal.\n");

	if (espconn_set_opt
	    (connection, ESPCONN_KEEPALIVE | ESPCONN_NODELAY)) {
		os_printf("tcp_connection: failed to set options.\n");
		return;
	}

	if (espconn_regist_sentcb(connection, sent_cb)) {
		os_printf
		    ("tcp_connection: failed to set receive callback.\n");
		return;
	}

	if (espconn_regist_recvcb(connection, recv_cb)) {
		os_printf
		    ("tcp_connection: failed to set receive callback.\n");
		return;
	}

	if (espconn_regist_disconcb(connection, disconnect_cb)) {
		os_printf
		    ("tcp_connection: failed to set disconnect callback.\n");
		return;
	}

	ready_to_send = true;
	tcp_up = true;
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
	queue_clear(packet_queue);
	ready_to_send = true;
	tcp_up = false;
	node_notify(TERMINAL_LOST);
}

static void ICACHE_FLASH_ATTR disconnect_cb(void *arg)
{
	os_printf("Terminal disconnected.\n");
	queue_clear(packet_queue);
	ready_to_send = true;
	tcp_up = false;
	node_notify(TERMINAL_LOST);
}

static void ICACHE_FLASH_ATTR recv_cb(void *connection, char *pdata,
				      unsigned short len)
{
	message_receiver_cb(pdata, len);
}

static void ICACHE_FLASH_ATTR do_send_packet(struct qsy_packet *packet)
{
	int8_t res;
	if ((res =
	     espconn_send(connection, (uint8_t *) packet,
			  QSY_PACKET_SIZE))) {
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

static void ICACHE_FLASH_ATTR sent_cb(void *connection)
{
	if (!queue_is_empty(packet_queue)) {
		queue_pop(packet_queue, &outgoing_packet);
		do_send_packet(&outgoing_packet);
	} else {
		ready_to_send = true;
	}
}

void ICACHE_FLASH_ATTR tcp_connection_send_packet(struct qsy_packet
						  *packet)
{
	if (!tcp_up) {
		os_printf("TCP connection not ready. Dropping...\n");
		return;
	}
	if (ready_to_send) {
		outgoing_packet = *packet;
		do_send_packet(&outgoing_packet);
		ready_to_send = false;
	} else {
		if (queue_is_full(packet_queue))
			os_printf("Packet queue full. Dropping packet!\n");
		else
			queue_push(packet_queue, packet);
	}

}
