#include "ets_sys.h"
#include "discovery.h"
#include "led.h"
#include "wifi.h"
#include "command.h"
#include "node.h"

void node_notify(uint16_t event)
{
        switch(event) {
        case GOT_IP:
                discovery_start();
                break;
        case GOT_TERMINAL:
                discovery_stop();
                break;
        case TERMINAL_LOST:
                discovery_stop();
                break;
        default:
                break;
        }
}

void ICACHE_FLASH_ATTR user_init()
{
	led_init();
	wifi_init();
        command_init();
}
