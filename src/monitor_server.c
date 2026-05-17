#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "shared_mem.h"
#include "message_queue.h"
#include "client_manager.h"

#define PORT 8080
#define MAX_EVENTS 100

sensor_data_t *shared_data;

message_queue_t queue;

pthread_mutex_t client_mutex =
    PTHREAD_MUTEX_INITIALIZER;

int running = 1;

void signal_handler(int sig)
{
    running = 0;
}

int set_nonblocking(int fd)
{
    int flags;

    flags = fcntl(fd,
                  F_GETFL,
                  0);

    if(flags < 0)
    {
        return -1;
    }

    return fcntl(fd,
                 F_SETFL,
                 flags | O_NONBLOCK);
}

void *producer_thread(void *arg)
{
    int last_temp = -1;
    int last_hum = -1;
    int last_press = -1;

    while(running)
    {
        pthread_mutex_lock(&shared_data->mutex);

        int temp =
            shared_data->temperature;

        int hum =
            shared_data->humidity;

        int press =
            shared_data->pressure;

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

        for(int i = 0;
            i < MAX_CLIENTS;
            i++)
        {
            if(clients[i].active == 0)
            {
                continue;
            }

            strncpy(clients[i].outbuf,
                    msg,
                    BUFFER_SIZE);

            clients[i].outlen =
                strlen(msg);
        }

        pthread_mutex_unlock(&client_mutex);
    }

    return NULL;
}

int main()
{
    signal(SIGINT,
           signal_handler);

    signal(SIGPIPE,
           SIG_IGN);

    queue_init(&queue);

    init_clients();

    shared_data =
        get_shared_memory();

    if(shared_data == NULL)
    {
        return -1;
    }

    int server_fd;

    server_fd =
        socket(AF_INET,
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

    memset(&addr,
           0,
           sizeof(addr));

    addr.sin_family = AF_INET;

    addr.sin_port =
        htons(PORT);

    addr.sin_addr.s_addr =
        INADDR_ANY;

    if(bind(server_fd,
            (struct sockaddr *)&addr,
            sizeof(addr)) < 0)
    {
        perror("bind");

        return -1;
    }

    if(listen(server_fd,
              10) < 0)
    {
        perror("listen");

        return -1;
    }

    printf("[SERVER] Listening on port %d\n",
           PORT);

    set_nonblocking(server_fd);

    int epfd;

    epfd = epoll_create1(0);

    if(epfd < 0)
    {
        perror("epoll_create1");

        return -1;
    }

    struct epoll_event ev;

    struct epoll_event events[MAX_EVENTS];

    ev.events = EPOLLIN;

    ev.data.fd = server_fd;

    epoll_ctl(epfd,
              EPOLL_CTL_ADD,
              server_fd,
              &ev);

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
        int nfds;

        nfds = epoll_wait(epfd,
                          events,
                          MAX_EVENTS,
                          -1);

        if(nfds < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }

            perror("epoll_wait");

            break;
        }

        for(int i = 0;
            i < nfds;
            i++)
        {
            if(events[i].data.fd ==
               server_fd)
            {
                struct sockaddr_in
                    client_addr;

                socklen_t client_len =
                    sizeof(client_addr);

                int client_fd;

                client_fd =
                    accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &client_len);

                if(client_fd < 0)
                {
                    perror("accept");

                    continue;
                }

                set_nonblocking(client_fd);

                int idx;

                idx =
                    add_client(client_fd);

                if(idx < 0)
                {
                    printf("[SERVER] Max clients reached\n");

                    close(client_fd);

                    continue;
                }

                printf("[SERVER] Client added idx=%d fd=%d\n",
                       idx,
                       client_fd);

                struct epoll_event
                    client_ev;

                client_ev.events =
                    EPOLLIN |
                    EPOLLOUT;

                client_ev.data.fd =
                    client_fd;

                epoll_ctl(epfd,
                          EPOLL_CTL_ADD,
                          client_fd,
                          &client_ev);
            }
            else
            {
                int fd =
                    events[i].data.fd;

                pthread_mutex_lock(&client_mutex);

                for(int j = 0;
                    j < MAX_CLIENTS;
                    j++)
                {
                    if(clients[j].active &&
                       clients[j].fd == fd)
                    {
                        if(clients[j].outlen > 0)
                        {
                            int ret;

                            ret =
                                send(fd,
                                     clients[j].outbuf,
                                     clients[j].outlen,
                                     0);

                            if(ret <= 0)
                            {
                                printf("[SERVER] Removing client fd=%d\n",
                                       fd);

                                remove_client(j);
                            }
                            else
                            {
                                clients[j].outlen = 0;
                            }
                        }
                    }
                }

                pthread_mutex_unlock(&client_mutex);
            }
        }
    }

    close(server_fd);

    return 0;
}