PJBASE=/home/napwob/pjproject-master

include $(PJBASE)/build.mak

CC      = $(APP_CC)
LDFLAGS = $(APP_LDFLAGS)
LDLIBS  = $(APP_LDLIBS)
CFLAGS  = $(APP_CFLAGS)
CPPFLAGS= ${CFLAGS}

all: clean sip_client

sip_client: main.c
	$(CC) pjstuff.c -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) `pkg-config --cflags --libs gtk+-3.0` -lncurses

clean:
	rm -f main.o sip_client
