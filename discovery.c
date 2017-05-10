#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "discovery.h"
#include "message.h"
#include "protocol.h"

/* In miliseconds */
#define PERIOD 500

/* Static functions */ 
static espconn_sent_callback sent_cb(void *arg);
static void ICACHE_FLASH_ATTR join_multicast(struct ip_addr * local, struct ip_addr * multicast);
static void ICACHE_FLASH_ATTR leave_multicast();
static void ICACHE_FLASH_ATTR setup_timer();
static void ICACHE_FLASH_ATTR disarm_timer();
static void timer_cb(void * arg);
static void ICACHE_FLASH_ATTR listen_udp();

/* Static variables */
static struct qsy_message hello_msg = 
{
	.signature={'Q','S','Y'},
	.type = HELLO_MSG
};
static esp_udp conf = {
	.remote_port = 3000,
	.local_port = 0
};
static struct espconn connection  =
{
	.type = ESPCONN_UDP,
	.state = ESPCONN_NONE,
	.proto.udp = &conf,
	.sent_callback = (espconn_sent_callback)sent_cb
};
static struct ip_addr multicast;
static volatile os_timer_t msg_timer;

void ICACHE_FLASH_ATTR discovery_start()
{
	listen_udp();
	join_multicast((ip_addr_t*)&conf.local_ip,&multicast);
	setup_timer();
        os_printf("Sending probes ...\n");
}

void ICACHE_FLASH_ATTR discovery_stop()
{
	leave_multicast();
	espconn_disconnect(&connection);
        espconn_delete(&connection);
	disarm_timer();
        os_printf("Discovery finished.\n");
}

static void ICACHE_FLASH_ATTR listen_udp()
{
	os_printf("Setting up UDP...\n");

	/* Wish I didn't have to do this */
        /* TODO fix this. */
        IP4_ADDR(&multicast, 224,0,0,12);
	os_memcpy(&conf.remote_ip, &multicast,4);

	switch(espconn_create(&connection)) {
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
}

static void ICACHE_FLASH_ATTR join_multicast(struct ip_addr * local, struct ip_addr * multicast)
{
	os_printf("Joining multicast...\n");
	if(espconn_igmp_join(local, multicast)) 
		os_printf("IGMP join failed\n");
}

static void ICACHE_FLASH_ATTR leave_multicast()
{
        if(espconn_igmp_leave((struct ip_addr*) &conf.local_ip, &multicast)) 
		os_printf("IGMP leave failed\n");
}

static espconn_sent_callback sent_cb(void *arg)
{
        return NULL;
}

static void timer_cb(void * arg)
{
	if(espconn_sendto(&connection, (void*) &hello_msg, QSY_MSG_SIZE)) {
		os_printf("espconn_sendto: failed!");
	}
}

static void ICACHE_FLASH_ATTR setup_timer()
{
	os_timer_disarm(&msg_timer);
	os_timer_setfn(&msg_timer, (os_timer_func_t*) timer_cb, NULL);
	os_timer_arm(&msg_timer, PERIOD, true);
} 

static void ICACHE_FLASH_ATTR disarm_timer()
{
	os_timer_disarm(&msg_timer);
}
