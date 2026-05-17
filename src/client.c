#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#include "protocol.h"

#define SERVER_IP "127.0.0.1"

#define PORT 8080

int main()
{
    int sockfd;

    sockfd =
        socket(AF_INET,
               SOCK_STREAM,
               0);

    if(sockfd < 0)
    {
        perror("socket");

        return -1;
    }

    struct sockaddr_in server_addr;

    memset(&server_addr,
           0,
           sizeof(server_addr));

    server_addr.sin_family =
        AF_INET;

    server_addr.sin_port =
        htons(PORT);

    inet_pton(AF_INET,
              SERVER_IP,
              &server_addr.sin_addr);

    if(connect(sockfd,
               (struct sockaddr *)&server_addr,
               sizeof(server_addr)) < 0)
    {
        perror("connect");

        return -1;
    }

    printf("[CLIENT] Connected to server\n");

    while(1)
    {
        char buffer[256];

        int n;

        n = recv(sockfd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

        if(n <= 0)
        {
            printf("[CLIENT] Server disconnected\n");

            break;
        }

        buffer[n] = '\0';

        printf("[CLIENT] Received: %s",
               buffer);

        /*
           heartbeat reply
        */

        if(is_ping_packet(buffer))
        {
            char pong[64];

            create_pong_packet(pong);

            send(sockfd,
                 pong,
                 strlen(pong),
                 0);

            printf("[CLIENT] Sent: %s",
                   pong);

            continue;
        }

        /*
           ACK reply
        */

        int msg_id;

        msg_id =
            parse_sensor_packet(buffer);

        if(msg_id >= 0)
        {
            char ackbuf[64];

            create_ack_packet(ackbuf,
                              msg_id);

            send(sockfd,
                 ackbuf,
                 strlen(ackbuf),
                 0);

            printf("[CLIENT] Sent: %s",
                   ackbuf);
        }
    }

    close(sockfd);

    return 0;
}