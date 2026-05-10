CC=gcc

CFLAGS=-Wall -lpthread -lrt -Iinclude

all:
	gcc src/shared_mem.c src/sensor_process.c -o sensor_process $(CFLAGS)

	gcc src/shared_mem.c src/message_queue.c src/monitor_server.c -o monitor_server $(CFLAGS)

	gcc src/client.c -o client $(CFLAGS)

clean:
	rm -f sensor_process monitor_server client
