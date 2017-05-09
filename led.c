#include <stdbool.h>
#include "ets_sys.h"
#include "led.h"
#include "gpio.h"
#include "eagle_soc.h"

#define LED_PIN 2

void led_init()
{
	gpio_init();
        gpio_output_set(0, 0 , (1 << LED_PIN), 0);
}


void led_set(uint16_t color)
{
	if(color)
		gpio_output_set((1 << LED_PIN), 0 , 0, 0); 
	else
		gpio_output_set(0, (1 << LED_PIN), 0, 0);     
}