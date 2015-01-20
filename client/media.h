#pragma once

#include "ClientSockfd.h"

#define NET_DISCOVER_AUDIO 1
#define NET_DISCOVER_VIDEO 2
#define NET_DISCOVER_ONLINE_FILE 3


extern int DealDicoverSendFeedback(CRPBaseHeader *, u_int32_t);

extern int DealAudioDicoverAcceptFeedback(CRPBaseHeader *);

extern void DealwithRequestAudioNetDiscover();

//一个标志位，用来显示现在是否在视频
extern int FlagVideo;

extern struct LogRequestFriendDiscover
{
    int uid;
    int requset_reason;
};

extern struct AudioDiscoverProcessEntry
{
    uint8_t key[32], peerKey[32];
    int peerKeySet, peerReady;
    uint32_t peerUid;
    uint8_t messageSent;
    struct sockaddr_in addr;
    session_id_t localSession, peerSession;
    pthread_t workerThread;
    FriendInfo *friendInfo;
};
extern struct LogRequestFriendDiscover the_log_request_friend_discover;


extern int popup_request_num_limit(gpointer);

extern gboolean TreatmentRequestVideoDiscover(gpointer);


extern int DealVideoDicoverServerFeedback(CRPBaseHeader *, void *);

extern int DealVideoFeedback(CRPBaseHeader *, void *);

extern int ProcessNatDiscoveredOnAudio(CRPBaseHeader *, void *);

extern int AudioAcceptNatDiscover(CRPPacketNETNATRequest *, FriendInfo *);

extern int AudioRequestNATDiscover(FriendInfo *);