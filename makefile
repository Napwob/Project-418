PJBASE=/home/napwob/pjproject-master

include $(PJBASE)/build.mak

CC      = $(APP_CC)
LDFLAGS = $(APP_LDFLAGS)
LDLIBS  = $(APP_LDLIBS)
CFLAGS  = $(APP_CFLAGS)
CPPFLAGS= ${CFLAGS}

all: sip_client

sip_client: main.c
	$(CC) pjstuff.c -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f main.o sip_client
