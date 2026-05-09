#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 8080

int main()
{
    int sockfd;

    sockfd = socket(AF_INET,
                    SOCK_STREAM,
                    0);

    if(sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET,
              "127.0.0.1",
              &server_addr.sin_addr);

    if(connect(sockfd,
               (struct sockaddr *)&server_addr,
               sizeof(server_addr)) < 0)
    {
        perror("connect");
        return -1;
    }

    printf("[CLIENT] Connected to server\n");

    char buffer[128];

    while(1)
    {
        memset(buffer, 0, sizeof(buffer));

        int n;

        n = recv(sockfd,
                 buffer,
                 sizeof(buffer),
                 0);

        if(n <= 0)
        {
            printf("[CLIENT] Server disconnected\n");
            break;
        }

        printf("%s", buffer);

        fflush(stdout);
    }

    close(sockfd);

    return 0;
}
