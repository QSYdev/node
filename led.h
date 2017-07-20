#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>
#include "color.h"


void led_init(void);

void led_set_color(struct color col);

void led_turn_off();

bool led_is_on();


#endif
