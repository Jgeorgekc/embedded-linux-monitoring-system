#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <time.h>

#define MAX_CLIENTS 1000

#define BUFFER_SIZE 1024

typedef struct
{
    int fd;

    int active;

    char outbuf[BUFFER_SIZE];

    int outlen;

    int write_offset;

    int last_msg_id;

    int last_ack_id;

    int retry_count;

    time_t last_heartbeat;

} client_t;

extern client_t clients[MAX_CLIENTS];

void init_clients();

int add_client(int fd);

void remove_client(int index);

#endif