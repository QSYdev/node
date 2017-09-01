#include "packet.h"
#include "network.h"
#include "espmissingincludes.h"
#include "ssid.h"

#define RED_COMPONENT(x)   (x) >> 12 & 0xF
#define GREEN_COMPONENT(x) (x) >> 8  & 0xF
#define BLUE_COMPONENT(x)  (x) >> 4  & 0xF
#define COLOR(r,g,b) ((((r) & 0xF) << 12) | (((g) & 0xF) << 8) | (((b) & 0xF) << 4))
#define DISTANCE_FLAG_BIT 0
#define SOUND_FLAG_BIT 1

struct qsy_packet_p {
	char signature[3];
	uint8_t type;
	uint16_t id;
	uint16_t color;
	uint32_t delay;
	uint16_t config;
} __attribute__ ((packed));

void ICACHE_FLASH_ATTR packet_init(struct qsy_packet *packet)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	ets_memset(p, 0, QSY_PACKET_SIZE);

	p->signature[0] = 'Q';
	p->signature[1] = 'S';
	p->signature[2] = 'Y';
	p->id = htons(NODE_ID);
}

enum packet_type ICACHE_FLASH_ATTR packet_get_type(struct qsy_packet *packet)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	return (enum packet_type) p->type;
}

struct color ICACHE_FLASH_ATTR packet_get_color(const struct qsy_packet *packet)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	struct color res;
	uint16_t hcolor = ntohs(p->color);

	res.red = RED_COMPONENT(hcolor);
	res.green = GREEN_COMPONENT(hcolor);
	res.blue = BLUE_COMPONENT(hcolor);
	return res;
}

uint32_t ICACHE_FLASH_ATTR packet_get_delay(const struct qsy_packet *packet)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	return ntohl(p->delay);
}

bool ICACHE_FLASH_ATTR packet_get_sound(const struct qsy_packet *packet)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	return (1 << SOUND_FLAG_BIT) & p->config;
}

bool ICACHE_FLASH_ATTR packet_get_distance(const struct qsy_packet *packet)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	return (1 << DISTANCE_FLAG_BIT) & p->config;
}

void ICACHE_FLASH_ATTR packet_set_sound(const struct qsy_packet *packet, bool value)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	if (value)
		p->config |=  1 << SOUND_FLAG_BIT;
	else
		p->config &= ~(1 << SOUND_FLAG_BIT);
}

void ICACHE_FLASH_ATTR packet_set_distance(const struct qsy_packet *packet, bool value)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	if (value)
		p->config |=  1 << DISTANCE_FLAG_BIT;
	else
		p->config &= ~(1 << DISTANCE_FLAG_BIT);
}

void ICACHE_FLASH_ATTR packet_set_type(struct qsy_packet *packet, enum packet_type type)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	p->type = (uint8_t) type;
}

void ICACHE_FLASH_ATTR packet_set_color(struct qsy_packet *packet, struct color c)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	uint16_t hcolor = COLOR(c.red, c.green, c.blue);
	p->color = htons(hcolor);
}

void ICACHE_FLASH_ATTR packet_set_delay(struct qsy_packet *packet, uint32_t delay)
{
	struct qsy_packet_p *p = (struct qsy_packet_p *) packet;
	p->delay = htonl(delay);
}

bool ICACHE_FLASH_ATTR packet_is_valid(const struct qsy_packet *packet)
{
	return packet->private[0] == 'Q' && packet->private[1] == 'S' &&
	    packet->private[2] == 'Y';
}
