#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "ssid.h"
#include "message.h"

static const int LED_PIN = 2;

char led_state = 0;

struct espconn connection;

esp_udp udp_config;

struct station_config wifi_config;

struct softap_config  ap_config;

static volatile os_timer_t my_timer;


espconn_recv_callback udp_receive_callback(void *arg, char *pdata, unsigned short len)
{

}

espconn_sent_callback udp_sent_callback(void *arg)
{
	os_printf("Sent something over UDP.\n");
	if(led_state)
	{
		os_printf("LED OFF\n");
		gpio_output_set(1 << LED_PIN,0,0,0);
	} else {
		os_printf("LED ON\n");
		gpio_output_set(0,1 << LED_PIN,0,0);
	}
	led_state = !led_state;
}

ip_addr_t multicast;

void ICACHE_FLASH_ATTR init_udp()
{
	sint8 result;
	
	IP4_ADDR(&multicast,224,0,0,12);
	
	// Setup UDP "connection"
	os_printf("Setting up UDP...\n");
	connection.type = ESPCONN_UDP;
	connection.state = ESPCONN_NONE;
	connection.proto.udp = &udp_config;

	udp_config.remote_port = 3000;
	udp_config.local_port = 0;

	os_memset(&udp_config.local_ip,0,4);
	os_memcpy(&udp_config.remote_ip,&multicast,4);
	connection.recv_callback = (espconn_recv_callback) udp_receive_callback;
	connection.sent_callback = (espconn_sent_callback) udp_sent_callback;

	if(result = espconn_create(&connection))
	{
		os_printf("espconn_create failed: \n");
		switch(result)
		{
			case ESPCONN_ISCONN:
			os_printf("UDP already established!\n");
			break;
			case ESPCONN_MEM:
			os_printf("Out of memory\n");
			break;
			case ESPCONN_ARG:
			os_printf("Illegal argument\n");
			break;
		}
		return;
	}

	// Join multicast
	os_printf("Joining multicast...\n");


	if(result = espconn_igmp_join((ip_addr_t*)&udp_config.local_ip, &multicast))
	{
		os_printf("IGMP join failed\n");
		return;
	}

	os_printf("Ready!\n");
}

void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt)
{
	if(evt->event == EVENT_STAMODE_GOT_IP)
	{
		os_printf("Got IP!\n");
		init_udp();
	}
}

void ICACHE_FLASH_ATTR init_wifi()
{
	os_printf("Setting up 802.11 ");

	os_printf("as station.");
	if(!wifi_set_opmode_current(1))
		os_printf("Failed to set 802.11 station mode!\n");

	os_memcpy(&wifi_config.ssid,SSID,32);
	os_memcpy(&wifi_config.password,PASSWORD,64);
	wifi_config.bssid_set = 0;

	wifi_set_event_handler_cb(wifi_handle_event_cb);

	if(!wifi_station_set_config_current(&wifi_config))
		os_printf("Failed to set 802.11 config!\n");
}

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t  user_procTaskQueue[user_procTaskQueueLen];
static void user_procTask(os_event_t *events);

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{
    os_delay_us(10);
}

struct qsy_message hello_msg;

void timer_callback(void *arg)
{	
	if(espconn_sendto(&connection, &hello_msg, QSY_MSG_SIZE))
	{
		os_printf("espconn_sendto failed!");
	}
}

void ICACHE_FLASH_ATTR user_init()
{
	gpio_init();
	
	hello_msg.signature[0] = 'Q';
	hello_msg.signature[1] = 'S';
	hello_msg.signature[2] = 'Y';
	hello_msg.type = HELLO_MSG;

	// configure LED pin
	gpio_output_set(0, 0, (1 << LED_PIN), 0);
	gpio_output_set(1 << LED_PIN, 0, 0, 0);

	init_wifi();
	
	//Disarm timer
    os_timer_disarm(&my_timer);

    //Setup timer
    os_timer_setfn(&my_timer, (os_timer_func_t *)timer_callback, NULL);

    //Arm the timer
    os_timer_arm(&my_timer, 2000, 1);
	
	//Start os task
    system_os_task(user_procTask, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
}
