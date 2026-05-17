#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#define MAX_CLIENTS 1000

#define BUFFER_SIZE 1024

typedef struct
{
    int fd;

    int active;

    char outbuf[BUFFER_SIZE];

    int outlen;

} client_t;

extern client_t clients[MAX_CLIENTS];

void init_clients();

int add_client(int fd);

void remove_client(int index);

#endif