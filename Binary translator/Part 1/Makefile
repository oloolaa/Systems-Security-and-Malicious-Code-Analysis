
CCFLAGS += -g -m32 

OBJDUMP = objdump

assign2: assign2.c assign2.S
	$(CC) $(CCFLAGS) assign2.c assign2.S -static -Wl,-N -o $@
	$(OBJDUMP) -d $@ > $@.dis

clean:
	rm -rf assign2 assign2.dis

