#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <stdlib.h>
#include<string.h>

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
    MD5((unsigned char *) "d", 1, hash);
    CRPLoginLoginSend(sockfd, "a", hash);

    log_info("Login", "Waiting OK\n");
    header = CRPRecv(sockfd);
    switch (header->packetID)
    {
        case CRP_PACKET_LOGIN_ACCEPT:
            log_info("Login", "Successful");
            break;
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *failure = CRPFailureCast(header);
            char *s = (char *) malloc(header->dataLength + 1);
            memcpy(s, failure->reason, header->dataLength);
            s[header->dataLength] = 0;
            log_error("Login", s);
            break;
        };
        default:
            log_error("Login", "Recv Packet:%d\n", header->packetID);
            break;

    }
    return 0;
}