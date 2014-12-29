#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <stdlib.h>
#include<string.h>
#include <protocol/base.h>

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
    CRPHelloSend(sockfd, 0, 1, 1, 1);
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
    MD5((unsigned char *) "s", 1, hash);
    CRPLoginRegisterSend(sockfd, 0, "1", hash, "nick");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK)
    {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 1;
    }
    CRPLoginLoginSend(sockfd, 0, "a", hash);

    log_info("Login", "Waiting OK\n");
    header = CRPRecv(sockfd);
    switch (header->packetID)
    {
        case CRP_PACKET_LOGIN_ACCEPT:
            log_info("Login", "Successful\n");
            break;
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *failure = CRPFailureCast(header);
            char *s = (char *) malloc(header->dataLength + 1);
            memcpy(s, failure->reason, header->dataLength);
            s[header->dataLength] = 0;
            log_error("Login", s);
            return 1;
        };
        default:
            log_error("Login", "Recv Packet:%d\n", header->packetID);
            return 1;

    }

    CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
    uint32_t uid = ac->uid;
    if (ac != header->data)
        free(ac);
    CRPInfoRequestSend(sockfd, 0, uid); //请求用户资料
    CRPFriendRequestSend(sockfd, 0);    //请求用户好友列表
    while (1)
    {
        header = CRPRecv(sockfd);
        switch (header->packetID)
        {
            case CRP_PACKET_INFO_DATA:
            {
                CRPPacketInfoData *infoData = CRPInfoDataCast(header);
                log_info("User", "Nick:%s\n", infoData->nickName);
                CRPFileRequestSend(sockfd, 10, 0, infoData->icon);

                if (infoData != header->data)
                    free(infoData);
                break;
            }
            case CRP_PACKET_FILE_DATA_START:
            {
                CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);
                log_info("Icon", "%lu bytes will be received\n", packet->dataLength);
                if (packet != header->data)
                    free(packet);
                break;
            };
            case CRP_PACKET_FILE_DATA:
                header->sessionID;
                log_info("Icon", "Recv data %lu bytes.\n", header->dataLength);
                break;
            case CRP_PACKET_FILE_DATA_END:
            {
                CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);
                if (packet->code == 0)
                {
                    log_info("Icon", "Recv Successful\n");
                }
                else
                {
                    log_info("Icon", "Recv Fail with code %d", (int) packet->code);
                }
                if (packet != header->data)
                    free(packet);
                break;
            }
            case CRP_PACKET_FRIEND_DATA:
            {
                UserFriends *friends = UserFriendsDecode((unsigned char *) header->data);
                log_info("Friends", "Group Count:%d\n", friends->groupCount);
                for (int i = 0; i < friends->groupCount; ++i)
                {
                    UserGroup *group = friends->groups + i;
                    log_info(group->groupName, "GroupID:%d\n", group->groupId);
                    log_info(group->groupName, "FriendCount:%d\n", group->friendCount);
                    for (int j = 0; j < group->friendCount; ++j)
                    {
                        CRPInfoRequestSend(sockfd, 1, group->friends[j]); //请求用户资料
                        log_info(group->groupName, "Friend:%u\n", group->friends[j]);
                    }
                }
                UserFriendsFree(friends);
                break;
            }
        }
        free(header);
    }
    return 0;
}