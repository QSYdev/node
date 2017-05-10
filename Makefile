CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls -DICACHE_FLASH -Wall
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

image: node
	esptool.py elf2image $^ 

node: node.o led.o discovery.o wifi.o command.o

node.o: node.c node.h

discovery.o: discovery.c discovery.h message.h protocol.h

led.o: led.c led.h

wifi.o: wifi.c wifi.h

command.o: command.c command.h

id.o: id.h

flash: image
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean: 
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
