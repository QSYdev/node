#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "c_types.h"
#include "message.h"

void tcp_connection_init(void);
void tcp_connection_send_message(struct qsy_message *message);

#endif
