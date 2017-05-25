#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "c_types.h"

void ICACHE_FLASH_ATTR tcp_connection_init(void);
void ICACHE_FLASH_ATTR tcp_connection_set_recv_callback(void); //TODO cambiar el parametro que recibe.

#endif
