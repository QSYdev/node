#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>
#include "color.h"


void led_init(void);

void led_set_color(struct color col);

void led_turn_on();

void led_turn_off();

void led_set_blink(uint16_t period);

bool led_is_on();


#endif
