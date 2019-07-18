LDFLAGS = -ljson-c 

wifi-probe:wifi-probe.o genl-lib.o
	$(CC) $(LDFLAGS) wifi-probe.o genl-lib.o -o wifi-probe
genl-lib.o:genl-lib.c
	$(CC) $(CFLAGS) -c genl-lib.c
wifi-probe.o:wifi-probe.c
	$(CC) $(CFLAGS) -c wifi-probe.c

clean:
	rm *.o wifi-probe
