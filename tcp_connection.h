#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "c_types.h"
#include "packet.h"

void tcp_connection_init(void);
void tcp_connection_send_packet(struct qsy_packet *packet);

#endif
