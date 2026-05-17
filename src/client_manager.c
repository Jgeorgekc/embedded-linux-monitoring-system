#include <unistd.h>
#include <string.h>

#include "client_manager.h"

client_t clients[MAX_CLIENTS];

void init_clients()
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;

        clients[i].active = 0;

        clients[i].outlen = 0;

        memset(clients[i].outbuf,
               0,
               BUFFER_SIZE);
    }
}

int add_client(int fd)
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(clients[i].active == 0)
        {
            clients[i].fd = fd;

            clients[i].active = 1;

            clients[i].outlen = 0;

            return i;
        }
    }

    return -1;
}

void remove_client(int index)
{
    close(clients[index].fd);

    clients[index].fd = -1;

    clients[index].active = 0;

    clients[index].outlen = 0;
}