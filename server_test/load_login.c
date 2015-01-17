#include<pthread.h>
#include<protocol.h>

#define CLIENT_COUNT 200

pthread_t child[CLIENT_COUNT];
unsigned char hash[16];

void processFailure(const char *name, const char *process, CRPBaseHeader *header)
{
    if (header == NULL)
    {
        log_info(name, "%s Recv Failure.\n", process);
    }
    else if (header->packetID == CRP_PACKET_FAILURE)
    {
        CRPPacketFailure *packet = CRPFailureCast(header);
        log_info(name, "%s\n", packet->reason);
        if ((void *) packet != header->data)
        {
            free(packet);
        }
    }
    else
    {
        log_info(name, "%s %hu\n", process, header->packetID);

    }
}

volatile int tCount = 0;

void *threadRoutine(void *p)
{
    char name[10];
    sprintf(name, "%lu", (pthread_t *) p - child);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            //.sin_addr.s_addr=inp.s_addr,
            .sin_port=htons(8014)
    };
    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
    {
        perror("Connect");
        return 0;
    }
    CRPContext crp = CRPOpen(fd);
    CRPHelloSend(crp, 0, 1, 1, 1, 1);
    ++tCount;
    CRPBaseHeader *header = NULL;
    header = CRPRecv(crp);
    if (header == NULL || header->packetID != CRP_PACKET_OK)
    {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 0;
    }
    char sendKey[32], iv[32];
    CRPSwitchProtocolSend(crp, 1, sendKey, iv);
    ++tCount;
    header = CRPRecv(crp);
    if (header->packetID != CRP_PACKET_SWITCH_PROTOCOL)
    {
        log_error("SwitchProtocol", "Can not enable encrypt!\n", header->packetID);
    }
    else
    {
        CRPPacketSwitchProtocol *packet = CRPSwitchProtocolCast(header);
        CRPEncryptEnable(crp, sendKey, packet->key, packet->iv);
        if ((void *) packet != header->data)
        {
            free(packet);
        }
    }
    CRPLoginLoginSend(crp, 0, name, hash);//发送用户名密码
    ++tCount;
    header = CRPRecv(crp);
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        log_info("登录失败", "登录失败\n");
        return NULL;
    }

    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT)
    {
        log_info("登录成功", "登录成功\n");

        //登陆成功之后开始请求资料
        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;   ///拿到用户uid
        free(header);
        if ((void *) ac != header->data)
        {
            free(ac);
        }


        CRPFriendRequestSend(crp, 1);  //请求用户好友列表
        ++tCount;
        int fileCount = 0;
        while (1)
        {
            header = CRPRecv(crp);
            switch (header->packetID)
            {
                case CRP_PACKET_FAILURE:
                {
                    CRPPacketFailure *failure = CRPFailureCast(header);
                    log_error("FAULT", failure->reason);
                    goto cleanup;
                };
                case CRP_PACKET_INFO_DATA: //用户资料回复
                {
                    CRPPacketInfoData *infoData = CRPInfoDataCast(header);
                    CRPFileRequestSend(crp, header->sessionID, 0, infoData->info.icon);//请求用户资料,通过ssionID区别
                    ++tCount;
                    if ((void *) infoData != header->data)
                    {
                        free(infoData);
                    }
                    break;
                }
                case CRP_PACKET_FILE_DATA_START://服务器准备发送头像
                {
                    ++fileCount;
                    CRPOKSend(crp, header->sessionID);
                    ++tCount;
                    break;
                };

                case CRP_PACKET_FILE_DATA://接受头像
                {
                    CRPOKSend(crp, header->sessionID);
                    ++tCount;
                    break;
                };

                case CRP_PACKET_FILE_DATA_END://头像接受完
                {
                    if (--fileCount == 0)
                    {
                        goto cleanup;
                    }
                    break;
                }
                case CRP_PACKET_FRIEND_DATA://分组
                {
                    UserFriends *friends = UserFriendsDecode((unsigned char *) header->data);
                    for (int i = 0; i < friends->groupCount; ++i)//循环组
                    {
                        UserGroup *group = friends->groups + i;

                        for (int j = 0; j < group->friendCount; ++j)//循环好友

                        {
                            CRPInfoRequestSend(crp, group->friends[j], group->friends[j]); //请求用户资料,
                            ++tCount;
                        }
                    }
                    UserFriendsFree(friends);
                    break;
                }
                default:

                    break;
            }
            free(header);
        }
    }
    cleanup:
    if (header)
    {
        free(header);
    }
    CRPClose(crp);
    return NULL;
}

int main()
{
    MD5((unsigned char *) "s", 1, hash);
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    for (int i = 0; i < CLIENT_COUNT; ++i)
    {
        pthread_create(child + i, NULL, threadRoutine, child + i);
    }
    for (int i = 0; i < CLIENT_COUNT; ++i)
    {
        pthread_join(child[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    log_info("Main", "Done\n");
    log_info("Main",
             "%d packet in %lds%ldns\n",
             tCount,
             endTime.tv_sec - startTime.tv_sec,
             endTime.tv_nsec - startTime.tv_nsec);
    //sleep(100);
    return 0;
}