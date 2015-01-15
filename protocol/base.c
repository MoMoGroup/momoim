#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <openssl/md5.h>

void *(*const PacketsDataCastMap[CRP_PACKET_ID_MAX + 1])(CRPBaseHeader *) = {
        [CRP_PACKET_KEEP_ALIVE]               = (void *(*)(CRPBaseHeader *)) CRPKeepAliveCast,
        [CRP_PACKET_HELLO]                    = (void *(*)(CRPBaseHeader *)) CRPHelloCast,
        [CRP_PACKET_FAILURE]                  = (void *(*)(CRPBaseHeader *)) CRPFailureCast,
        [CRP_PACKET_OK]                       = (void *(*)(CRPBaseHeader *)) CRPOKCast,
        [CRP_PACKET_KICK]                     = (void *(*)(CRPBaseHeader *)) CRPKickCast,
        [CRP_PACKET_CANCEL]                   = (void *(*)(CRPBaseHeader *)) CRPCancelCast,
        [CRP_PACKET_SWITCH_PROTOCOL]          = (void *(*)(CRPBaseHeader *)) CRPSwitchProtocolCast,
        [CRP_PACKET_CRASH]                    = (void *(*)(CRPBaseHeader *)) CRPCrashCast,

        [CRP_PACKET_LOGIN__START]             = (void *(*)(CRPBaseHeader *)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]              = (void *(*)(CRPBaseHeader *)) CRPLoginLoginCast,
        [CRP_PACKET_LOGIN_ACCEPT]             = (void *(*)(CRPBaseHeader *)) CRPLoginAcceptCast,
        [CRP_PACKET_LOGIN_LOGOUT]             = (void *(*)(CRPBaseHeader *)) CRPLoginLogoutCast,
        [CRP_PACKET_LOGIN_REGISTER]           = (void *(*)(CRPBaseHeader *)) CRPLoginRegisterCast,

        [CRP_PACKET_INFO__START]              = (void *(*)(CRPBaseHeader *)) NULL,
        [CRP_PACKET_INFO_REQUEST]             = (void *(*)(CRPBaseHeader *)) CRPInfoRequestCast,
        [CRP_PACKET_INFO_DATA]                = (void *(*)(CRPBaseHeader *)) CRPInfoDataCast,
        [CRP_PACKET_INFO_STATUS_CHANGE]       = (void *(*)(CRPBaseHeader *)) CRPInfoStatusChangeCast,
        [CRP_PACKET_INFO_PASSWORD_CHANGE]     = (void *(*)(CRPBaseHeader *)) CRPInfoPasswordChangeCast,

        [CRP_PACKET_FRIEND__START]            = (void *(*)(CRPBaseHeader *)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]           = (void *(*)(CRPBaseHeader *)) CRPFriendRequestCast,
        [CRP_PACKET_FRIEND_DATA]              = (void *(*)(CRPBaseHeader *)) CRPFriendDataCast,
        [CRP_PACKET_FRIEND_NOTIFY]            = (void *(*)(CRPBaseHeader *)) CRPFriendNotifyCast,
        [CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME]= (void *(*)(CRPBaseHeader *)) CRPFriendSearchByNicknameCast,
        [CRP_PACKET_FRIEND_USER_LIST]         = (void *(*)(CRPBaseHeader *)) CRPFriendUserListCast,
        [CRP_PACKET_FRIEND_ADD]               = (void *(*)(CRPBaseHeader *)) CRPFriendAddCast,
        [CRP_PACKET_FRIEND_ACCEPT]            = (void *(*)(CRPBaseHeader *)) CRPFriendAcceptCast,
        [CRP_PACKET_FRIEND_MOVE]              = (void *(*)(CRPBaseHeader *)) CRPFriendMoveCast,
        [CRP_PACKET_FRIEND_DELETE]            = (void *(*)(CRPBaseHeader *)) CRPFriendDeleteCast,
        [CRP_PACKET_FRIEND_GROUP_ADD]         = (void *(*)(CRPBaseHeader *)) CRPFriendFriendGroupAddCast,
        [CRP_PACKET_FRIEND_GROUP_RENAME]      = (void *(*)(CRPBaseHeader *)) CRPFriendFriendGroupRenameCast,
        [CRP_PACKET_FRIEND_GROUP_DELETE]      = (void *(*)(CRPBaseHeader *)) CRPFriendFriendGroupDeleteCast,
        [CRP_PACKET_FRIEND_GROUP_MOVE]        = (void *(*)(CRPBaseHeader *)) CRPFriendFriendGroupMoveCast,

        [CRP_PACKET_FILE__START]              = (void *(*)(CRPBaseHeader *)) NULL,
        [CRP_PACKET_FILE_REQUEST]             = (void *(*)(CRPBaseHeader *)) CRPFileRequestCast,
        [CRP_PACKET_FILE_DATA]                = (void *(*)(CRPBaseHeader *)) CRPFileDataCast,
        [CRP_PACKET_FILE_DATA_START]          = (void *(*)(CRPBaseHeader *)) CRPFileDataStartCast,
        [CRP_PACKET_FILE_DATA_END]            = (void *(*)(CRPBaseHeader *)) CRPFileDataEndCast,
        [CRP_PACKET_FILE_RESET]               = (void *(*)(CRPBaseHeader *)) CRPFileResetCast,
        [CRP_PACKET_FILE_STORE_REQUEST]       = (void *(*)(CRPBaseHeader *)) CRPFileStoreRequestCast,
        [CRP_PACKET_FILE_PROXY]               = (void *(*)(CRPBaseHeader *)) CRPFileProxyRequestCast,

        [CRP_PACKET_MESSAGE__START]           = (void *(*)(CRPBaseHeader *)) NULL,
        [CRP_PACKET_MESSAGE_NORMAL]           = (void *(*)(CRPBaseHeader *)) CRPMessageNormalCast,
        [CRP_PACKET_MESSAGE_QUERY_OFFLINE]    = (void *(*)(CRPBaseHeader *)) CRPMessageQueryOfflineCast,
        [CRP_PACKET_MESSAGE_RECORD_QUERY]     = (void *(*)(CRPBaseHeader *)) CRPMessageRecordQueryCast,
        [CRP_PACKET_MESSAGE_RECORD_DATA]      = (void *(*)(CRPBaseHeader *)) CRPMessageRecordDataCast,

        [CRP_PACKET_NET__START]               = (void *(*)(CRPBaseHeader *)) NULL,
        [CRP_PACKET_NET_QUALITY_TEST]         = (void *(*)(CRPBaseHeader *)) CRPNETQualityTestCast,
        [CRP_PACKET_NET_INET_ADDRESS]         = (void *(*)(CRPBaseHeader *)) CRPNETInetAddressCast,
        [CRP_PACKET_NET_FRIEND_DISCOVER]      = (void *(*)(CRPBaseHeader *)) CRPNETFriendDiscoverCast,
        [CRP_PACKET_NET_DISCOVER_ACCEPT]      = (void *(*)(CRPBaseHeader *)) CRPNETDiscoverAcceptCast,
        [CRP_PACKET_NET_DISCOVER_REFUSE]      = (void *(*)(CRPBaseHeader *)) CRPNETDiscoverRefuseCast,
        [CRP_PACKET_NET_NAT_DISCOVER]         = (void *(*)(CRPBaseHeader *)) CRPNATDiscoverCast,
        [CRP_PACKET_NET_DETECTED]             = (void *(*)(CRPBaseHeader *)) CRPNATDetectedCast,
};

typedef struct
{
    uint8_t type;
    uint16_t seq;
    uint16_t length;
    uint8_t payload[0];
} ARQPacket;
static pthread_t arqThread;
static pthread_mutex_t arqLock;

static void ARQInit()
{
    //TOD!O ARQ
}

static void CRPEncryptDisableUnlock(CRPContext context)
{
    if (context->sendTd || context->recvTd)
    {
        mcrypt_generic_deinit(context->sendTd);
        mcrypt_module_close(context->sendTd);
        context->sendTd = NULL;

        mcrypt_generic_deinit(context->recvTd);
        mcrypt_module_close(context->recvTd);
        context->recvTd = NULL;
    }
}

CRPContext CRPOpen(int fd)
{
    CRPContext context = (CRPContext) malloc(sizeof(__CRPContext));
    //int state = 1;
    //setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
    context->fd = fd;
    context->sendTd = NULL;
    context->recvTd = NULL;
    pthread_mutex_init(&context->sendLock, NULL);
    pthread_mutex_init(&context->recvLock, NULL);

    return context;
}

int CRPClose(CRPContext context)
{
    if (!context)
    {
        return 0;
    }
    CRPEncryptDisableUnlock(context);
    shutdown(context->fd, SHUT_RDWR);
    close(context->fd);
    pthread_mutex_destroy(&context->sendLock);
    pthread_mutex_destroy(&context->recvLock);
    free(context);
    return 1;
}

int CRPEncryptTest(const char key[32], const char iv[32])
{
    int ret = 0;
    char *cpKey = (char *) malloc(32),
            *cpIV = (char *) malloc(32);
    if (cpKey == NULL || cpIV == NULL)
    {
        goto cleanup;
    }
    MCRYPT td = mcrypt_module_open(MCRYPT_RIJNDAEL_256, NULL, MCRYPT_CBC, NULL);
    if (td == MCRYPT_FAILED)
    {
        goto cleanup;
    }
    if (32 != mcrypt_enc_get_key_size(td) || 32 != mcrypt_enc_get_iv_size(td) || 32 != mcrypt_enc_get_block_size(td))
    {
        goto cleanup;
    }
    if (0 != mcrypt_enc_self_test(td))
    {
        goto cleanup;
    }
    memcpy(cpKey, key, 32);
    memcpy(cpIV, iv, 32);
    int r = mcrypt_generic_init(td, cpKey, 32, cpIV);
    if (r < 0)
    {
        goto cleanup;
    }
    mcrypt_generic_deinit(td);
    ret = 1;
    cleanup:
    if (td)
    {
        mcrypt_module_close(td);
    }
    free(cpKey);
    free(cpIV);
    return ret;
}

int CRPEncryptEnable(CRPContext context, const char sendKey[32], const char recvKey[32], const char iv[32])
{
    int retcode = 0;
    pthread_mutex_lock(&context->sendLock);
    pthread_mutex_lock(&context->recvLock);
    if (context->sendTd || context->recvTd)
    {
        CRPEncryptDisableUnlock(context);
    }
    MCRYPT sendTd = mcrypt_module_open(MCRYPT_RIJNDAEL_256, NULL, MCRYPT_CBC, NULL);
    if (sendTd == MCRYPT_FAILED)
    {
        goto cleanup;
    }
    if (mcrypt_enc_get_key_size(sendTd) != 32 || mcrypt_enc_get_iv_size(sendTd) != 32 || mcrypt_enc_get_block_size(
            sendTd) != 32)
    {
        fprintf(stderr, "CRP Warning: Cannot enable data encrypt!\nKey Size Error\n");
        mcrypt_module_close(sendTd);
        goto cleanup;
    }
    MD5((unsigned char *) sendKey, 32, (unsigned char *) context->sendKey);
    MD5((unsigned char *) context->sendKey, 16, (unsigned char *) context->sendKey + 16);
    MD5((unsigned char *) recvKey, 32, (unsigned char *) context->recvKey);
    MD5((unsigned char *) context->recvKey, 16, (unsigned char *) context->recvKey + 16);
    //memcpy(context->sendKey, sendKey, 32);
    //memcpy(context->recvKey, recvKey, 32);
    memcpy(&context->sendIV, iv, 32);
    memcpy(&context->recvIV, iv, 32);

    retcode = mcrypt_generic_init(sendTd, context->sendKey, 32, context->sendIV);
    if (retcode < 0)
    {
        fprintf(stderr, "CRP Warning: Cannot enable data encrypt!\nmcrypt_generic_init return:%d\n", retcode);
        mcrypt_module_close(sendTd);
        goto cleanup;
    }
    MCRYPT recvTd = mcrypt_module_open(MCRYPT_RIJNDAEL_256, NULL, MCRYPT_CBC, NULL);
    if (recvTd == MCRYPT_FAILED)
    {
        mcrypt_generic_deinit(sendTd);
        mcrypt_module_close(sendTd);
        goto cleanup;
    }

    retcode = mcrypt_generic_init(recvTd, context->recvKey, 32, context->recvIV);
    if (retcode < 0)
    {
        fprintf(stderr, "CRP Warning: Cannot enable data encrypt!\nmcrypt_generic_init return:%d\n", retcode);
        mcrypt_generic_deinit(sendTd);
        mcrypt_module_close(sendTd);
        mcrypt_module_close(recvTd);
        goto cleanup;
    }

    context->sendTd = sendTd;
    context->recvTd = recvTd;
    cleanup:
    pthread_mutex_unlock(&context->recvLock);
    pthread_mutex_unlock(&context->sendLock);
    return retcode;
}

void CRPEncryptDisable(CRPContext context)
{
    pthread_mutex_lock(&context->recvLock);
    pthread_mutex_lock(&context->sendLock);
    CRPEncryptDisableUnlock(context);
    pthread_mutex_unlock(&context->sendLock);
    pthread_mutex_unlock(&context->recvLock);
}

ssize_t CRPSend(CRPContext context,
                packet_id_t packetID,
                session_id_t sessionID,
                void const *data,
                CRP_LENGTH_TYPE dataLength)
{
    void *packet;
    CRPBaseHeader *header;
    CRP_LENGTH_TYPE protocolLength = (CRP_LENGTH_TYPE) (sizeof(CRPBaseHeader) + dataLength),
            encryptedLength = protocolLength,
            fullLength;
    ssize_t ret = 0;

    pthread_mutex_lock(&context->sendLock);

    if (context->sendTd != NULL)
    {
        if (protocolLength % 32 != 0)
        {
            encryptedLength += 32 - protocolLength % 32;
        }
        fullLength = (CRP_LENGTH_TYPE) (sizeof(CRP_LENGTH_TYPE) + encryptedLength);
        packet = malloc(fullLength);
        header = (CRPBaseHeader *) (packet + sizeof(CRP_LENGTH_TYPE));
        CRP_LENGTH_TYPE draw = encryptedLength >> 5;
        memcpy(packet, &draw, sizeof(CRP_LENGTH_TYPE));
        bzero((char *) (header) + protocolLength, encryptedLength - protocolLength);
    }
    else
    {
        fullLength = protocolLength;
        packet = header = malloc(protocolLength);
    }

    header->magicCode = 0x464F5573;
    header->totalLength = (CRP_LENGTH_TYPE) protocolLength;
    header->packetID = packetID;
    header->sessionID = sessionID;
    if (dataLength)
    {
        memcpy(header->data, data, dataLength);
    }

    if (context->sendTd)
    {
        if (0 != mcrypt_generic(context->sendTd, header, (int) encryptedLength))
        {
            fprintf(stderr, "CRP Fault: Cannot encrypt data using CRPContext.\n");
            abort();
        }
    }
    while (-1 == (ret = send(context->fd, packet, fullLength, 0/*MSG_MORE*/)) &&
            (errno == EWOULDBLOCK || errno == EAGAIN))
    {
        //如果系统要求重发则阻塞当前线程,等待重发
        fd_set fdWr, fdEx;
        FD_ZERO(&fdWr);
        FD_SET(context->fd, &fdWr);
        fdEx = fdWr;
        struct timeval time = {
                .tv_sec=3   //当前线程只停留3秒钟.3秒后还无法发送数据,本次发送失败.
        };
        int n = select(context->fd + 1, NULL, &fdWr, &fdEx, &time);
        if (n == -1 || FD_ISSET(context->fd, &fdEx))//如果select失败或者fd异常,本次发送失败.警告用户.
        {
            perror("Warning: Fail to send packet");
            goto cleanup;
        }
        //其他情况都重试发送
    }

    cleanup:
    pthread_mutex_unlock(&context->sendLock);
    if (packet)
    {
        free(packet);
    }
    return ret;
}

CRPBaseHeader *CRPRecv(CRPContext context)
{
    CRPBaseHeader *packet = NULL;
    ssize_t ret;
    pthread_mutex_lock(&context->recvLock);
    if (context->recvTd)//接收通道被加密时
    {
        CRP_LENGTH_TYPE encryptedLength;
        ret = recv(context->fd, &encryptedLength, sizeof(CRP_LENGTH_TYPE), MSG_WAITALL);
        if (ret != sizeof(encryptedLength))
        {
            return NULL;
        }
        encryptedLength <<= 5;
        packet = (CRPBaseHeader *) malloc(encryptedLength);
        ret = recv(context->fd, packet, encryptedLength, MSG_WAITALL);
        if (ret != encryptedLength)
        {
            free(packet);
            return NULL;
        }
        if (0 != mdecrypt_generic(context->recvTd, packet, encryptedLength))
        {
            free(packet);
            return NULL;
        }
        if (packet->magicCode != 0x464F5573)
        {
            free(packet);
            return NULL;
        }
    }
    else
    {   //接收通道未加密时
        CRPBaseHeader h;
        ret = recv(context->fd, &h, sizeof(CRPBaseHeader), MSG_PEEK | MSG_WAITALL);
        if (ret != sizeof(CRPBaseHeader) || h.magicCode != 0x464F5573)
        {
            fprintf(stderr, "ret:%d,errno:%d,strerror:%s\n", (int) ret, errno, strerror(errno));
            return NULL;
        }
        packet = (CRPBaseHeader *) malloc(h.totalLength);
        ret = recv(context->fd, packet, h.totalLength, MSG_WAITALL);
        if (ret != h.totalLength)
        {
            fprintf(stderr, "Recv Error 2-%d bytes.%d/%s\n", (int) ret, errno, strerror(errno));
            free(packet);
            return NULL;
        }
    }
    pthread_mutex_unlock(&context->recvLock);
    return packet;
}