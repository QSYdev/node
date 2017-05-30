CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls -DICACHE_FLASH -Wall
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

node-0x00000.bin: node
	esptool.py elf2image $^

node: hello.o led.o node.o wifi.o udp_connection.o tcp_connection.o sensor.o command.o message_receiver.o

hello.o: hello.c hello.h message.h protocol.h udp_connection.h

led.o: led.c led.h

node.o: node.c node.h led.h wifi.h tcp_connection.h udp_connection.h sensor.h hello.h command.h message_receiver.h

wifi.o: wifi.c wifi.h ssid.h node.h

udp_connection.o: udp_connection.c udp_connection.h message.h protocol.h

tcp_connection.o: tcp_connection.c tcp_connection.h network.h protocol.h message.h node.h message_receiver.h

sensor.o: sensor.c sensor.h

command.o: command.c command.h network.h protocol.h led.h

message_receiver.o: message_receiver.c message_receiver.h command.h network.h protocol.h message.h

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
