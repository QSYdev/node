#include "osapi.h"
#include "user_interface.h"
#include "ssid.h"
#include "wifi.h"
#include "node.h"

static void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt);

static struct station_config wifi_config;

void ICACHE_FLASH_ATTR wifi_init()
{
	os_printf("wifi_init: Setting up 802.11 to station mode ... ");
	if(!wifi_set_opmode_current(1))
		os_printf("Failed to set 802.11 station mode!\n");

	os_memcpy(&wifi_config.ssid, SSID, 32);
	os_memcpy(&wifi_config.password, PASSWORD,64);
	wifi_config.bssid_set = 0;

	wifi_set_event_handler_cb(wifi_handle_event_cb);

	if(!wifi_station_set_config_current(&wifi_config))
		os_printf("Failed to set 802.11 config!\n");
}


void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt)
{
	if(evt->event == EVENT_STAMODE_GOT_IP) {
		os_printf("Got IP!\n");
		node_notify(GOT_IP);
	}
}