#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init();
void led_set(uint16_t color);
void led_blink_start(uint16_t color, uint16_t interval);
void led_blink_stop();

#endif