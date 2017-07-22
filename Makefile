CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls -DICACHE_FLASH -DNODE_ID=$(id) -Wall -Wno-missing-braces -Os
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lpwm -lc -Wl,--end-group -lgcc
LDFLAGS = -Tmy.eagle.app.v6.ld

node-0x00000.bin: node
	esptool.py elf2image $^

node: hello.o led.o node.o wifi.o udp_connection.o tcp_connection.o sensor.o command.o message_receiver.o keep_alive.o queue.o packet.o

hello.o: hello.c hello.h packet.h protocol.h udp_connection.h network.h

led.o: led.c led.h

node.o: node.c node.h led.h wifi.h tcp_connection.h udp_connection.h sensor.h hello.h command.h message_receiver.h

wifi.o: wifi.c wifi.h ssid.h node.h

udp_connection.o: udp_connection.c udp_connection.h packet.h protocol.h

tcp_connection.o: tcp_connection.c tcp_connection.h network.h protocol.h packet.h node.h message_receiver.h queue.h

sensor.o: sensor.c sensor.h

command.o: command.c command.h network.h protocol.h led.h

message_receiver.o: message_receiver.c message_receiver.h command.h network.h protocol.h packet.h

keep_alive.o: keep_alive.c keep_alive.h tcp_connection.h packet.h network.h

queue.o: queue.c queue.h

packet.o: packet.c packet.h

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
