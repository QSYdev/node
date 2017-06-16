#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#define GOT_IP         0
#define GOT_TERMINAL   1
#define TERMINAL_LOST  2

void node_notify(uint16_t event);

#endif	/* */
