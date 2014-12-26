#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
    {
        perror("Connect");
        return 1;
    }
    log_info("Hello", "Sending Hello\n");
    CRPHelloSend(sockfd, 1, 1, 1);
    CRPBaseHeader *header;
    log_info("Hello", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK)
    {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 1;
    }

    log_info("Login", "Sending Login Request\n");
    unsigned char hash[16];
    MD5((unsigned char *) "pass", 4, hash);
    CRPLoginLoginSend(sockfd, "username", hash);
    if (header->packetID != CRP_PACKET_OK)
    {
        log_error("Login", "Recv Packet:%d\n", header->packetID);
        return 1;
    }
    log_info("Login", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK)
    {
        log_error("Login", "Recv Packet:%d\n", header->packetID);
        return 1;
    }
    else
    {
        log_info("Login", "Login Done\n");
    }
    return 0;
}