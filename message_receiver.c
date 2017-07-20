#include "message_receiver.h"
#include "command.h"
#include "packet.h"

void message_receiver_cb(char *pdata, unsigned short length)
{
	if (length == QSY_PACKET_SIZE) {
		struct qsy_packet *packet = (struct qsy_packet *) pdata;

		if (packet_is_valid(packet)
		    && packet_get_type(packet) == command)
			command_packet_received(pdata);
	}
}
