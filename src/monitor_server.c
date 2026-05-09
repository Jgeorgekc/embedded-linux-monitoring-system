#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#include "shared_mem.h"

#define PORT 8080

sensor_data_t *shared_data;

int running = 1;

void signal_handler(int sig)
{
    running = 0;
}

int main()
{
    signal(SIGINT, signal_handler);

    shared_data = get_shared_memory();

    if(shared_data == NULL)
    {
        return -1;
    }

    int server_fd;

    server_fd = socket(AF_INET,
                       SOCK_STREAM,
                       0);

    if(server_fd < 0)
    {
        perror("socket");
        return -1;
    }

    int opt = 1;

    setsockopt(server_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_fd,
            (struct sockaddr *)&addr,
            sizeof(addr)) < 0)
    {
        perror("bind");
        return -1;
    }

    if(listen(server_fd, 10) < 0)
    {
        perror("listen");
        return -1;
    }

    printf("[SERVER] Listening on port %d\n", PORT);

    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(client_addr);

    int client_fd;

    client_fd = accept(server_fd,
                       (struct sockaddr *)&client_addr,
                       &client_len);

    if(client_fd < 0)
    {
        perror("accept");
        return -1;
    }

    printf("[SERVER] Client connected\n");

    while(running)
    {
        pthread_mutex_lock(&shared_data->mutex);

        char buffer[128];

        snprintf(buffer,
                 sizeof(buffer),
                 "TEMP=%d HUM=%d PRESS=%d\n",
                 shared_data->temperature,
                 shared_data->humidity,
                 shared_data->pressure);

        pthread_mutex_unlock(&shared_data->mutex);

        printf("[SERVER] Sending: %s", buffer);

        fflush(stdout);

        int ret;

        ret = send(client_fd,
                   buffer,
                   strlen(buffer),
                   0);

        if(ret <= 0)
        {
            printf("[SERVER] Client disconnected\n");
            break;
        }

        sleep(1);
    }

    close(client_fd);
    close(server_fd);

    return 0;
}
