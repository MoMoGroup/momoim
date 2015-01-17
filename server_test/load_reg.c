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
    free(header);
    CRPLoginRegisterSend(crp, 0, name, hash, name);
    ++tCount;
    header = CRPRecv(crp);
    if (header == NULL || header->packetID == CRP_PACKET_FAILURE)
    {
        log_info("登录失败", "注册失败\n");
        return NULL;
    }
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
    return 0;
}