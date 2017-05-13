#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init();
void led_set(uint16_t color);
void led_toggle();

#endif