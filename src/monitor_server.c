#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "shared_mem.h"
#include "message_queue.h"

#define PORT 8080

sensor_data_t *shared_data;
message_queue_t queue;

#define MAX_CLIENTS 100

int clients[MAX_CLIENTS];

int client_count = 0;

pthread_mutex_t client_mutex =
    PTHREAD_MUTEX_INITIALIZER;
    

int running = 1;
int client_id = 0;

void signal_handler(int sig)
{
    running = 0;
}

void *producer_thread(void *arg)
{
    int last_temp = -1;
    int last_hum = -1;
    int last_press = -1;

    while(running)
    {
        pthread_mutex_lock(&shared_data->mutex);

        int temp = shared_data->temperature;
        int hum = shared_data->humidity;
        int press = shared_data->pressure;

        pthread_mutex_unlock(&shared_data->mutex);

        if(temp != last_temp ||
           hum != last_hum ||
           press != last_press)
        {
            char buffer[256];

            snprintf(buffer,
                     sizeof(buffer),
                     "TEMP=%d HUM=%d PRESS=%d\n",
                     temp,
                     hum,
                     press);

            enqueue(&queue,
                    buffer);

            printf("[QUEUE] Enqueued: %s",
                   buffer);

            last_temp = temp;
            last_hum = hum;
            last_press = press;
        }

        sleep(1);
    }

    return NULL;
}
void *consumer_thread(void *arg)
{
    while(running)
    {
        char msg[256];

        dequeue(&queue,
                msg);

        printf("[QUEUE] Broadcasting: %s",
        msg);

        pthread_mutex_lock(&client_mutex);

        for(int i = 0; i < client_count; i++)
        {
            int ret;

            if(clients[i] == -1)
            {
                continue;
            }
            ret = send(clients[i],
                    msg,
                    strlen(msg),
                    0);

            if(ret <= 0)
            {
                printf("[SERVER] Client disconnected\n");
                printf("[SERVER] Removing client %d\n",
                clients[i]);

                close(clients[i]);

                clients[i] = -1;
            }
        }

        pthread_mutex_unlock(&client_mutex);

        /*
           ivide later:
           broadcast to all clients
           epoll integration
        */
    }

    return NULL;
}
int main()
{
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, SIG_IGN);
    
    queue_init(&queue);

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

    pthread_t prod_tid;
    pthread_t cons_tid;

    pthread_create(&prod_tid,
               NULL,
               producer_thread,
               NULL);

    pthread_create(&cons_tid,
               NULL,
               consumer_thread,
               NULL);

    while(running)
    {
        struct sockaddr_in client_addr;

        socklen_t client_len =
            sizeof(client_addr);

        int client_fd;

        client_fd = accept(server_fd,
                        (struct sockaddr *)&client_addr,
                        &client_len);

        if(client_fd < 0)
        {
            perror("accept");

            continue;
        }

        printf("[SERVER] New client connected\n");

        pthread_mutex_lock(&client_mutex);

        clients[client_count++] = client_fd;

        pthread_mutex_unlock(&client_mutex);

    }
    close(server_fd);

    return 0;
}
