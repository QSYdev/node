#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include "c_types.h"

void ICACHE_FLASH_ATTR udp_connection_init(void);
void ICACHE_FLASH_ATTR udp_connection_send_message(void *message,
						   int length);
void ICACHE_FLASH_ATTR udp_connection_stop(void);

#endif
