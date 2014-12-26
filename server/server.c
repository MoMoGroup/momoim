#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <logger.h>

int server_exit = 0;

pthread_t ThreadListener;
#define LISTEN_PORT 8014

int main(int argc, char **argv)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(LISTEN_PORT),
            .sin_addr.s_addr = htons(INADDR_ANY)
    };

    int on = 1;
    if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt reuse address");
        goto failexit;
    }
    if (-1 == bind(sockfd, (struct sockaddr *) &addr, sizeof addr))
    {
        perror("bind");
        goto failexit;
    }

    if (-1 == listen(sockfd, 50))
    {
        perror("listen");
        goto failexit;
    }
    log_info("SERVER-MAIN", "Listenning on TCP %d\n", LISTEN_PORT);
    pthread_create(&ThreadListener, NULL, ListenMain, &sockfd);
    pthread_join(ThreadListener, NULL);
    close(sockfd);
    return EXIT_SUCCESS;

    failexit:
    close(sockfd);
    return EXIT_FAILURE;
}