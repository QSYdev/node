#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <stdbool.h>
#include "color.h"

#define QSY_PACKET_SIZE 12

struct qsy_packet {
	char private[QSY_PACKET_SIZE];
} __attribute__ ((packed));

enum packet_type {
	hello = 0,
	command = 1,
	touche = 2,
	keepalive = 3
};

void packet_init(struct qsy_packet *packet);

enum packet_type packet_get_type(struct qsy_packet *packet);

struct color packet_get_color(const struct qsy_packet *packet);

uint32_t packet_get_delay(const struct qsy_packet *packet);

void packet_set_type(struct qsy_packet *packet, enum packet_type type);

void packet_set_color(struct qsy_packet *packet, struct color c);

void packet_set_delay(struct qsy_packet *packet, uint32_t delay);

bool packet_is_valid(const struct qsy_packet *packet);

#endif
