#include "ets_sys.h"
#include "gpio.h"
#include "led.h"
#include "wifi.h"
#include "tcp_connection.h"
#include "udp_connection.h"
#include "node.h"
#include "sensor.h"
#include "hello.h"
#include "command.h"
#include "keep_alive.h"
#include "espmissingincludes.h"

#define BLINK_PERIOD 250

void node_notify(uint16_t event)
{
	switch (event) {
	case GOT_IP:
	case TERMINAL_LOST:
		keep_alive_stop();
		command_stop();
		hello_start();
		led_init();
		struct color color;
		color.red = 0;
		color.green = 0;
		color.blue = 15;
		led_set_color(color);
		led_set_blink(BLINK_PERIOD);
		break;
	case GOT_TERMINAL:
		hello_stop();
		command_start();
		keep_alive_start();
		led_set_blink(0);
		led_turn_off();
		break;
	default:
		break;
	}
}

void ICACHE_FLASH_ATTR user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);

	gpio_init();
	tcp_connection_init();
	led_init();
	wifi_init();
	sensor_init();
}
