#include "message_receiver.h"
#include "command.h"

void message_receiver_cb(char* pdata, unsigned short length) {
    if(length == QSY_MSG_SIZE) {
		struct qsy_message* msg = (struct qsy_message*) pdata;

		if (msg->signature[0] == 'Q' && msg->signature[1] == 'S' && msg->signature[2] == 'Y') {
			switch(msg->type) {
			case CMD_MSG: {
                command_message_received(pdata);
                break;
            }
            }
        }
    }

}
