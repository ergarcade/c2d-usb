CC=gcc
PROG=c2d-usb

CFLAGS=-I./hidapi -I./src -g
LFLAGS=-g
LDFLAGS=`pkg-config libudev --libs` -lrt

OBJS=./src/$(PROG).o ./hidapi/hid.o ./src/util.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

stream-overlay: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS)
	rm -f $(PROG)
