#include <unistd.h>
#include <string.h>
#include <time.h>

#include "client_manager.h"

client_t clients[MAX_CLIENTS];

void init_clients()
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;

        clients[i].active = 0;

        clients[i].outlen = 0;

        clients[i].write_offset = 0;

        clients[i].last_msg_id = 0;

        clients[i].last_ack_id = 0;

        clients[i].retry_count = 0;

        clients[i].last_heartbeat =
            time(NULL);

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

            clients[i].write_offset = 0;

            clients[i].last_msg_id = 0;

            clients[i].last_ack_id = 0;

            clients[i].retry_count = 0;

            clients[i].last_heartbeat =
                time(NULL);

            memset(clients[i].outbuf,
                   0,
                   BUFFER_SIZE);

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

    clients[index].write_offset = 0;

    clients[index].last_msg_id = 0;

    clients[index].last_ack_id = 0;

    clients[index].retry_count = 0;

    memset(clients[index].outbuf,
           0,
           BUFFER_SIZE);
}