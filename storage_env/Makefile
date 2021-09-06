OBJS = main.c pthread_client_request.c pthread_refresh.c pthread_sqlite.c pthread_transfer.c pthread_analysis.c pthread_buzzer.c pthread_led.c pthread_camera.c link_list.c 
CC = gcc
CFLAGS += -pthread -lsqlite3

storage:$(OBJS)
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm *.o storage

