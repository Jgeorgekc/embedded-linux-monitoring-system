CC=gcc

CFLAGS=-Wall -lpthread -lrt -Iinclude

all:
	$(CC) src/shared_mem.c \
	      src/sensor_process.c \
	      -o sensor_process \
	      $(CFLAGS)

	$(CC) src/shared_mem.c \
	      src/message_queue.c \
	      src/client_manager.c \
	      src/protocol.c \
	      src/monitor_server.c \
	      -o monitor_server \
	      $(CFLAGS)

	$(CC) src/protocol.c \
	      src/client.c \
	      -o client \
	      $(CFLAGS)

clean:
	rm -f sensor_process
	rm -f monitor_server
	rm -f client