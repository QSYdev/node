CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

image: node
	esptool.py elf2image $^ 

node: node.o

node.o: node.c

flash: image
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean: 
	rm -vf node-0x00000.bin node-0x10000.bin node.o