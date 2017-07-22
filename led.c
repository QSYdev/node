#include <stdbool.h>
#include "osapi.h"
#include "os_type.h"
#include "ets_sys.h"
#include "eagle_soc.h"
#include "led.h"
#include "gpio.h"
#include "espmissingincludes.h"
#include "pwm.h"

/*
 * Sobre PWM: el período se setea en microsegundos. Los ciclos de trabajo se 
 * setean en unidades de 40 ns (no en porcentajes), pero la implementación de
 * Espressif tiene una limitación que hace que todo ciclo de trabajo mayor a 90%
 * sea 100%.
 */

#define CHANNEL_COUNT 3
#define PWM_PERIOD 10000
#define MAX_DUTY (PWM_PERIOD * 1000 / 40)
#define RED_CHANNEL   0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL  2

static bool on;
static struct color color;
static os_timer_t blink_timer;
static uint32_t pwm_conf[CHANNEL_COUNT][3] = {
	{PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, 4}, /* rojo  */
	{PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5, 5}, /* verde */
	{PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2, 2}  /* azul  */
};
static uint32_t duty_cycles[CHANNEL_COUNT] = {MAX_DUTY, MAX_DUTY, MAX_DUTY};

static void blink_callback(void *arg);

void ICACHE_FLASH_ATTR led_init(void)
{
	color.blue = 0xF;
	pwm_init(PWM_PERIOD, duty_cycles, CHANNEL_COUNT, pwm_conf);
	led_turn_off();
}

void ICACHE_FLASH_ATTR led_set_color(struct color col)
{
	color = col;
	if (on) {
		led_turn_on();
	}
}

/* Configura y prende */
void ICACHE_FLASH_ATTR led_turn_on()
{
	pwm_set_duty(MAX_DUTY - MAX_DUTY/16 * color.red, RED_CHANNEL);
	pwm_set_duty(MAX_DUTY - MAX_DUTY/16 * color.green, GREEN_CHANNEL);
	pwm_set_duty(MAX_DUTY - MAX_DUTY/16 * color.blue, BLUE_CHANNEL);
	pwm_start();
	on = true;
}

void ICACHE_FLASH_ATTR led_turn_off()
{
	pwm_set_duty(MAX_DUTY, RED_CHANNEL);
	pwm_set_duty(MAX_DUTY, GREEN_CHANNEL);
	pwm_set_duty(MAX_DUTY, BLUE_CHANNEL);
	pwm_start();
	on = false;
}

void ICACHE_FLASH_ATTR led_set_blink(uint16_t period)
{
	if (period == 0) {
		os_timer_disarm(&blink_timer);
	} else {
		os_timer_setfn(&blink_timer, blink_callback, NULL);
		os_timer_arm(&blink_timer, period, true);
	}
}

static void blink_callback(void *arg)
{
	on ? led_turn_off() : led_turn_on();
}
