#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "udp_connection.h"
#include "message.h"
#include "protocol.h"
#include "espmissingincludes.h"

static espconn_sent_callback sent_cb(void *arg);

static esp_udp conf = {
	.remote_port = 3000,
	.local_port = 0
};

static struct espconn connection = {
	.type = ESPCONN_UDP,
	.state = ESPCONN_NONE,
	.proto.udp = &conf,
	.sent_callback = (espconn_sent_callback) sent_cb
};

static struct ip_addr multicast;

void ICACHE_FLASH_ATTR udp_connection_init(void)
{
	os_printf("Setting up UDP...\n");

	IP4_ADDR(&multicast, 224, 0, 0, 12);
	os_memcpy(&conf.remote_ip, &multicast, 4);

	switch (espconn_create(&connection)) {
	case 0:
		os_printf("UDP socket OK.\n");
		break;
	case ESPCONN_ISCONN:
		os_printf("espconn_create: UDP already established.\n");
		break;
	case ESPCONN_MEM:
		os_printf("espconn_create: out of memory.\n");
		break;
	case ESPCONN_ARG:
		os_printf("espconn_create: illegal argument.\n");
		break;
	}

	os_printf("Joining multicast...\n");
	if (espconn_igmp_join((ip_addr_t *) & conf.local_ip, &multicast)) {
		os_printf("IGMP join failed\n");
	}
}

void ICACHE_FLASH_ATTR udp_connection_send_message(void *message,
						   int length)
{
	if (espconn_sendto(&connection, message, length)) {
		os_printf("espconn_sendto: failed!");
	}
}

void ICACHE_FLASH_ATTR udp_connection_stop(void)
{
	if (espconn_igmp_leave
	    ((struct ip_addr *) &conf.local_ip, &multicast)) {
		os_printf("IGMP leave failed\n");
	}

	espconn_disconnect(&connection);
	espconn_delete(&connection);
	os_printf("Discovery finished.\n");
}

static espconn_sent_callback sent_cb(void *arg)
{
	return NULL;
}
