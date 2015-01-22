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
        [CRP_PACKET_NET_NAT_REGISTER]         = (void *(*)(CRPBaseHeader *)) CRPNETNATRegisterCast,
        [CRP_PACKET_NET_NAT_REQUEST]          = (void *(*)(CRPBaseHeader *)) CRPNETNATRequestCast,
        [CRP_PACKET_NET_NAT_ACCEPT]           = (void *(*)(CRPBaseHeader *)) CRPNETNATAcceptCast,
        [CRP_PACKET_NET_NAT_REFUSE]           = (void *(*)(CRPBaseHeader *)) CRPNETNATRefuseCast,
        [CRP_PACKET_NET_NAT_DETECTED]         = (void *(*)(CRPBaseHeader *)) CRPNATDetectedCast,
        [CRP_PACKET_NET_NAT_READY]            = (void *(*)(CRPBaseHeader *)) CRPNETNATReadyCast,
};

//禁用网络协议加密功能.
//参数: CRPContext context - CRP句柄
//注意: 本函数不对CRP句柄进行锁定,本函数非线程安全!
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

// 从socket文件描述符上打开一个CRP句柄.
// 参数: int fd - socket文件描述符
// 返回: 成功返回CRP句柄,失败返回NULL
// 注意: 该socket将被CRP托管,由非CRP函数向该socket写入数据将导致数据错误.
CRPContext CRPOpen(int fd)
{
    CRPContext context = (CRPContext) malloc(sizeof(__CRPContext));
    //int state = 1;
    //setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
    if (context)
    {
        context->fd = fd;
        context->sendTd = NULL;
        context->recvTd = NULL;
        context->buffer = NULL;
        context->bLengthAct = 0;
        pthread_mutex_init(&context->sendLock, NULL);
        pthread_mutex_init(&context->recvLock, NULL);
    }
    return context;
}

// 关闭一个CRP句柄.
// 参数: CRPContext context - CRP上下文
// 返回: 成功返回1,失败返回0
// 注意:该函数也会关闭socket
int CRPClose(CRPContext context)
{
    if (!context)
    {
        return 0;
    }
    CRPEncryptDisableUnlock(context);
    shutdown(context->fd, SHUT_RDWR);
    close(context->fd);
    if (context->buffer)
    {
        free(context->buffer);
    }
    pthread_mutex_destroy(&context->sendLock);
    pthread_mutex_destroy(&context->recvLock);
    free(context);
    return 1;
}

// 测试是否支持加密
// 参数: const char key[32], 32字节AES密钥
// 参数: const char iv[32], 32字节IV初始向量
// 返回值:可加密返回1,不支持加密返回0
int CRPEncryptTest(const char key[32], const char iv[32])
{
    int ret = 0;
    char *cpKey = (char *) malloc(32),
            *cpIV = (char *) malloc(32);
    if (cpKey == NULL || cpIV == NULL)
    {
        goto cleanup;
    }
    //打开Rijndael-256加密模块
    MCRYPT td = mcrypt_module_open(MCRYPT_RIJNDAEL_256, NULL, MCRYPT_CBC, NULL);
    //模块验证
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
    //初始化加密模块
    int r = mcrypt_generic_init(td, cpKey, 32, cpIV);
    if (r < 0)
    {
        goto cleanup;
    }
    //反初始化模块
    mcrypt_generic_deinit(td);
    ret = 1;

    //清理资源
    cleanup:
    if (td)
    {
        mcrypt_module_close(td);
    }
    free(cpKey);
    free(cpIV);
    return ret;
}

// 激活CRP加密功能.如果函数失败,不会对CRP句柄造成任何影响
// 参数: CRPContext context - CRP上下文
// 参数: const char sendKey[32] - 发送通道密钥
// 参数: const char recvKey[32] - 接收通道密钥
// 参数: const char iv[32] - 初始化向量
// 返回值: 失败返回<0,其他值表成功
int CRPEncryptEnable(CRPContext context, const char sendKey[32], const char recvKey[32], const char iv[32])
{
    int retcode = -1;
    //锁定发送和接收通道
    pthread_mutex_lock(&context->sendLock);
    pthread_mutex_lock(&context->recvLock);
    //如果之前已加密,先解除加密
    if (context->sendTd || context->recvTd)
    {
        goto cleanup;//CRPEncryptDisableUnlock(context);
    }
    //激活发送加密
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
    //产生AES密钥
    MD5((unsigned char *) sendKey, 32, (unsigned char *) context->sendKey);
    MD5((unsigned char *) context->sendKey, 16, (unsigned char *) context->sendKey + 16);
    MD5((unsigned char *) recvKey, 32, (unsigned char *) context->recvKey);
    MD5((unsigned char *) context->recvKey, 16, (unsigned char *) context->recvKey + 16);
    //memcpy(context->sendKey, sendKey, 32);
    //memcpy(context->recvKey, recvKey, 32);
    //IV赋值
    memcpy(&context->sendIV, iv, 32);
    memcpy(&context->recvIV, iv, 32);
    //激活加密通道
    retcode = mcrypt_generic_init(sendTd, context->sendKey, 32, context->sendIV);
    if (retcode < 0)
    {
        fprintf(stderr, "CRP Warning: Cannot enable data encrypt!\nmcrypt_generic_init return:%d\n", retcode);
        mcrypt_module_close(sendTd);
        goto cleanup;
    }

    //激活接收加密
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
    //清理
    cleanup:
    pthread_mutex_unlock(&context->recvLock);
    pthread_mutex_unlock(&context->sendLock);
    return retcode;
}

// 解除CRP加密功能.
// 参数: CRPContext context - CRP上下文
void CRPEncryptDisable(CRPContext context)
{
    pthread_mutex_lock(&context->recvLock);
    pthread_mutex_lock(&context->sendLock);
    CRPEncryptDisableUnlock(context);
    pthread_mutex_unlock(&context->sendLock);
    pthread_mutex_unlock(&context->recvLock);
}

// 发送一个数据包
// 参数:CRPContext context - CRP句柄
// 参数:packet_id_t packetID - 包ID
// 参数:session_id_t sessionID - 会话ID
// 参数:void const *data - 数据指针
// 参数:CRP_LENGTH_TYPE length - 数据长度
// 返回:函数返回-1表失败,其他表成功
ssize_t CRPSend(CRPContext context,
                packet_id_t packetID,
                session_id_t sessionID,
                void const *data,
                CRP_LENGTH_TYPE length)
{
    void *packet;
    CRPBaseHeader *header;
    //长度信息
    CRP_LENGTH_TYPE protocolLength = (CRP_LENGTH_TYPE) (sizeof(CRPBaseHeader) + length),
            encryptedLength = protocolLength,
            fullLength;
    ssize_t ret = 0;
    //锁定加密通道
    pthread_mutex_lock(&context->sendLock);
    //如果当前句柄发送通道未加密.
    if (context->sendTd != NULL)
    {
        //RIJNDAEL-256 CBC模式要求32字节块对齐
        //32字节块补齐
        if (protocolLength % 32 != 0)
        {
            encryptedLength += 32 - protocolLength % 32;
        }
        //在包头前方增加包大小字段
        fullLength = (CRP_LENGTH_TYPE) (sizeof(CRP_LENGTH_TYPE) + encryptedLength);
        packet = malloc(fullLength);
        header = (CRPBaseHeader *) ((uint8_t *) packet + sizeof(CRP_LENGTH_TYPE));
        //由于激活加密时是32字节按块加密的,所以长度值后5位都是0,在这里直接移除掉后5位
        CRP_LENGTH_TYPE encLength = encryptedLength >> 5;
        memcpy(packet, &encLength, sizeof(CRP_LENGTH_TYPE));
        //对其部分清零
        bzero((char *) (header) + protocolLength, encryptedLength - protocolLength);
    }
    else
    {
        //不加密的话就直接发数据包了.
        fullLength = protocolLength;
        packet = header = malloc(protocolLength);
    }

    //校验魔数 - 由于TCP协议保证了数据有序,完整.加入魔数意义在于,判断CRP协议本身是否出错了.
    //如果协议本身出错,控制数据包体就不再有效,这时双方必须重新建立连接.
    header->magicCode = 0x464F5573;
    //数据包总长度
    header->totalLength = (CRP_LENGTH_TYPE) protocolLength;
    //包ID用来表明数据包功能
    header->packetID = packetID;
    //会话ID用来支持并行数据包处理
    header->sessionID = sessionID;
    //如果有数据包内容部分,还要拷贝内容
    if (length)
    {
        memcpy(header->data, data, length);
    }
    //如果发送通道加密了
    if (context->sendTd)
    {
        //加密数据包
        if (0 != mcrypt_generic(context->sendTd, header, (int) encryptedLength))
        {
            fprintf(stderr, "CRP Fault: Cannot encrypt data using CRPContext.\n");
            abort();
        }
    }
    //尝试发送数据
    while (-1 == (ret = send(context->fd, packet, fullLength, MSG_NOSIGNAL/*MSG_MORE*/)) &&
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
        if (n <= 0 || FD_ISSET(context->fd, &fdEx))//如果select失败或者fd异常,本次发送失败.警告用户.
        {
            perror("CRPWarning: Fail to send packet");
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

void CRPSetupBuffer(CRPContext context)
{
    context->buffer = (uint8_t *) malloc(INT16_MAX);
    context->bLengthAct = 0;
}

// 接收一个数据包
// 参数:CRPContext context - CRP句柄
// 返回:函数返回NULL表失败,其他表成功
CRPBaseHeader *CRPRecv(CRPContext context)
{
    CRPBaseHeader *packet = NULL;
    ssize_t ret;
    //锁定接收通道
    pthread_mutex_lock(&context->recvLock);
    if (context->recvTd)//接收通道被加密时
    {
        //接收加密块长度
        CRP_LENGTH_TYPE encryptedLength;
        if (context->buffer)
        {
            if (context->bLengthAct < sizeof(CRP_LENGTH_TYPE))
            {
                ret = recv(context->fd,
                           context->buffer + context->bLengthAct,
                           sizeof(CRP_LENGTH_TYPE) - context->bLengthAct,
                           0);
                if (ret <= 0)
                {
                    packet = NULL;
                    goto cleanup;
                }
                context->bLengthAct += ret;
                if (context->bLengthAct < sizeof(CRP_LENGTH_TYPE))
                {
                    errno = EAGAIN;
                    packet = NULL;
                    goto cleanup;
                }
            }
            memcpy(&encryptedLength, context->buffer, sizeof(CRP_LENGTH_TYPE));
            if (context->bLengthAct < encryptedLength)
            {
                ret = recv(context->fd,
                           context->buffer + context->bLengthAct,
                           encryptedLength - context->bLengthAct,
                           0);
                if (ret <= 0)
                {
                    packet = NULL;
                    goto cleanup;
                }
                context->bLengthAct += ret;

                if (context->bLengthAct < encryptedLength + sizeof(CRP_LENGTH_TYPE))
                {
                    errno = EAGAIN;
                    packet = NULL;
                    goto cleanup;
                }
            }
            packet = (CRPBaseHeader *) malloc(encryptedLength);
            memcpy(packet, context->buffer + sizeof(CRP_LENGTH_TYPE), encryptedLength);
            context->bLengthAct = 0;
        }
        else
        {
            ret = recv(context->fd, &encryptedLength, sizeof(CRP_LENGTH_TYPE), MSG_WAITALL);
            if (ret != sizeof(encryptedLength))
            {
                packet = NULL;
                goto cleanup;
            }
            encryptedLength <<= 5;
            packet = (CRPBaseHeader *) malloc(encryptedLength);
            //接收数据包实际内容
            size_t remain = encryptedLength;
            while (remain > 0)
            {
                ret = recv(context->fd, packet, remain, MSG_WAITALL);
                if (ret <= 0)
                {
                    free(packet);
                    packet = NULL;
                    goto cleanup;
                }
                remain -= ret;
            }

            if (ret != encryptedLength)
            {
                free(packet);
                packet = NULL;
                goto cleanup;
            }
        }
        //尝试解密数据包
        if (0 != mdecrypt_generic(context->recvTd, packet, encryptedLength))
        {
            free(packet);
            packet = NULL;
            goto cleanup;
        }
        //魔数校验,判断解密成功还是失败
        if (packet->magicCode != 0x464F5573)
        {
            free(packet);
            packet = NULL;
            goto cleanup;
        }
    }
    else
    {   //接收通道未加密时
        CRPBaseHeader h;
        if (context->buffer)
        {
            if (context->bLengthAct < sizeof(CRPBaseHeader))
            {
                ret = recv(context->fd,
                           context->buffer + context->bLengthAct,
                           sizeof(CRPBaseHeader) - context->bLengthAct,
                           0);
                if (ret <= 0)
                {
                    packet = NULL;
                    goto cleanup;
                }
                context->bLengthAct += ret;
                if (context->bLengthAct < sizeof(CRPBaseHeader))
                {
                    errno = EAGAIN;
                    packet = NULL;
                    goto cleanup;
                }
            }
            packet = (CRPBaseHeader *) context->buffer;
            if (context->bLengthAct < packet->totalLength)
            {
                ret = recv(context->fd,
                           context->buffer + context->bLengthAct,
                           packet->totalLength - context->bLengthAct,
                           0);
                if (ret <= 0)
                {
                    packet = NULL;
                    goto cleanup;
                }
                context->bLengthAct += ret;

                if (context->bLengthAct < packet->totalLength)
                {
                    errno = EAGAIN;
                    packet = NULL;
                    goto cleanup;
                }
            }
            packet = (CRPBaseHeader *) malloc(packet->totalLength);
            memcpy(packet, context->buffer, context->bLengthAct);
            context->bLengthAct = 0;
        }
        else
        {
            ret = recv(context->fd, &h, sizeof(CRPBaseHeader), MSG_PEEK | MSG_WAITALL);
            if (ret != sizeof(CRPBaseHeader) || h.magicCode != 0x464F5573)
            {
                fprintf(stderr, "ret:%d,errno:%d,strerror:%s\n", (int) ret, errno, strerror(errno));
                return NULL;
            }
            packet = (CRPBaseHeader *) malloc(h.totalLength);
            size_t remain = h.totalLength;
            while (remain > 0)
            {
                ret = recv(context->fd, packet, remain, MSG_WAITALL);
                if (ret <= 0)
                {
                    perror("recv");
                    free(packet);
                    return NULL;
                }
                remain -= ret;
            }
        }
    }
    cleanup:
    pthread_mutex_unlock(&context->recvLock);
    return packet;
}