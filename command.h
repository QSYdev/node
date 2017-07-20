#ifndef COMMAND_H
#define COMMAND_H

void command_start(void);
void command_stop(void);
void command_packet_received(char *pdata);
void command_touched(void);

#endif
