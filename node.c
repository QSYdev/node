#include "ets_sys.h"
#include "led.h"
#include "wifi.h"
#include "tcp_connection.h"
#include "udp_connection.h"
#include "node.h"
#include "sensor.h"
#include "hello.h"
#include "command.h"
#include "keep_alive.h"

void node_notify(uint16_t event)
{
	switch (event) {
	case GOT_IP:
	case TERMINAL_LOST:
		hello_start();
		command_stop();
		//keep_alive_stop();
		break;
	case GOT_TERMINAL:
		hello_stop();
		command_start();
		//keep_alive_start();
		break;
	default:
		break;
	}
}

void ICACHE_FLASH_ATTR user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);

	tcp_connection_init();
	led_init();
	wifi_init();
	sensor_init();
}
