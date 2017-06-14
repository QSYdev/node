#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "c_types.h"

void ICACHE_FLASH_ATTR tcp_connection_init(void);
void ICACHE_FLASH_ATTR tcp_connection_send_message(void *message, int length);

#endif
