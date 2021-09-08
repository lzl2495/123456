ARCH ?=
OBJS = main.c pthread_client_request.c pthread_refresh.c pthread_sqlite.c pthread_transfer.c pthread_analysis.c pthread_buzzer.c pthread_led.c pthread_camera.c link_list.c sqlite_link_list.c pthread_client_request.c pthread_M0_control.c
SQLITE = -L../../../cangku/cgi/lib -lsqlite3 -I../../../cangku/cgi/include
CFLAGS += -lpthread  
ifeq ($(ARCH), arm)
CC = arm-linux-gnueabihf-gcc
storage:$(OBJS)
	$(CC) $^ $(CFLAGS) $(SQLITE)  -o $@

else
CC = gcc
CFLAGS += -lsqlite3
storage:$(OBJS)
	$(CC) $^ $(CFLAGS) -Wall  -o $@
endif



clean:
	rm *.o storage

cp:
	cp storage ~/nfs/rootfs/
