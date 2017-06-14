#include <stdint.h>
#ifndef MESSAGE_H
#define MESSAGE_H

#define HELLO_MSG       0
#define CMD_MSG         1
#define TOUCHE_MSG      2
#define KEEP_ALIVE_MSG  3
    
#define RED_MASK    0x000F
#define GREEN_MASK  0x00F0
#define BLUE_MASK   0x0F00
#define WHITE_MASK  0xF000
    
#define RED_COMPONENT(x)   (x)     & 0xF
#define GREEN_COMPONENT(x) (x)>>4  & 0xF
#define BLUE_COMPONENT(x)  (x)>>8  & 0xF
#define WHITE_COMPONENT(x) (x)>>12 & 0xF
    
#define COLOR(r,g,b,w) (  (r)        & RED_MASK  ) | \
(((g) << 4) & GREEN_MASK) | \
    (((b) << 8) & BLUE_MASK) | \
    (((w) << 12) & WHITE_MASK) 
 
struct qsy_message {	
	char signature[3];
	uint8_t type;	
	uint16_t id;	
	uint16_t color;
	uint32_t delay;
} __attribute__ ((packed));

#define QSY_MSG_SIZE 12
 
#endif
