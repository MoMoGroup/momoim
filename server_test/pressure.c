#define CLIENT_COUNT 100

pthread_t child[CLIENT_COUNT];
unsigned char hash[16];

void processFailure(const char *name, const char *process, CRPBaseHeader *header)
{
    if (header == NULL) {
        log_info(name, "%s Recv Failure.\n", process);
    }
    else if (header->packetID == CRP_PACKET_FAILURE) {
        CRPPacketFailure *packet = CRPFailureCast(header);
        log_info(name, "%s\n", packet->reason);
        if ((void *) packet != header->data)
            free(packet);
    }
    else {
        log_info(name, "%s %hu\n", process, header->packetID);

    }
}

volatile int tCount = 0;

void *threadRoutine(void *p)
{
    char name[10];
    sprintf(name, "%lu", (pthread_t *) p - child);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct timeval timeout = {10, 0};
//    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Connect");
        return 0;
    }
    CRPHelloSend(sockfd, 0, 1, 1, 1);
    CRPBaseHeader *header;

    header = CRPRecv(sockfd);
    if (header == NULL || header->packetID != CRP_PACKET_OK) {
        processFailure(name, "Hello", header);
        return 0;
    }
    free(header);
    CRPLoginRegisterSend(sockfd, 0, name, hash, name);
    header = CRPRecv(sockfd);
    if (header == NULL || header->packetID != CRP_PACKET_OK) {
        processFailure(name, "Register", header);
        return 0;
    }
    free(header);
    CRPLoginLoginSend(sockfd, 0, name, hash);

    header = CRPRecv(sockfd);
    if (header == NULL || header->packetID != CRP_PACKET_LOGIN_ACCEPT) {
        processFailure(name, "Login", header);
        return 0;
    }
    CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
    uint32_t uid = ac->uid;
    if ((void *) ac != header->data)
        free(ac);
    free(header);
    CRPInfoRequestSend(sockfd, 0, uid); //请求用户资料
    //CRPFriendRequestSend(sockfd, 0);    //请求用户好友列表
    int count = 0;
    while (1) {
        header = CRPRecv(sockfd);
        if (header == NULL) {
            processFailure(name, "General", header);
            return 0;
        }
        switch (header->packetID) {
            case CRP_PACKET_INFO_DATA: {
                CRPPacketInfoData *infoData = CRPInfoDataCast(header);
                count++;
                CRPFileRequestSend(sockfd, 10, FST_SHARED, infoData->info.icon);
                if ((const char *) infoData != header->data)
                    free(infoData);
                break;
            }
            case CRP_PACKET_FILE_DATA_START:
                CRPOKSend(sockfd, header->sessionID);
                break;
            case CRP_PACKET_FILE_DATA:
                CRPOKSend(sockfd, header->sessionID);
                break;
            case CRP_PACKET_FILE_DATA_END: {
                CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);
                if (packet->code != 0) {
                    log_info(name, "Icon Failure\n");
                    goto cleanup;
                }
                if ((const char *) packet != header->data)
                    free(packet);
                count--;
                if (count == 0) {
                    goto cleanup;
                }
                break;
            }
            case CRP_PACKET_FRIEND_DATA: {
                UserFriends *friends = UserFriendsDecode((unsigned char *) header->data);
                for (int i = 0; i < friends->groupCount; ++i) {
                    UserGroup *group = friends->groups + i;
                    for (int j = 0; j < group->friendCount; ++j) {
                        CRPInfoRequestSend(sockfd, 1, group->friends[j]); //请求用户资料
                    }
                }
                UserFriendsFree(friends);
                break;
            }
        }
        free(header);
        header = NULL;
    }
    cleanup:
    if (header)
        free(header);
    close(sockfd);
    return NULL;
}

int main()
{
    MD5((unsigned char *) "s", 1, hash);
    for (int i = 0; i < CLIENT_COUNT; ++i) {
        pthread_create(child + i, NULL, threadRoutine, child + i);
    }
    for (int i = 0; i < CLIENT_COUNT; ++i) {
        pthread_join(child[i], NULL);
    }
    log_info("All", "Done\n");
    //sleep(100);
    return 0;
}