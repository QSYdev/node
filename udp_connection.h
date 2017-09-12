#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include "c_types.h"

void udp_connection_init(void);
void udp_connection_send_message(void *message, int length);
void udp_connection_stop(void);

#endif
