#pragma once

#define NET_DISCOVER_AUDIO 1
#define NET_DISCOVER_VIDEO 2
#define NET_DISCOVER_ONLINE_FILE 3


extern int deal_dicover_send_feedback(CRPBaseHeader *, u_int32_t );
extern void dealwith_request_audio_net_discover();
//struct Audio_Request_Info
//{
//    int audiocount;
//    int charlen;
//    char *audio_data;
//    uint32_t uid;
//};
struct log_request_friend_discover{
    uint32_t uid;
    int requset_reason;
};
extern struct log_request_friend_discover the_log_request_friend_discover;
void  audio_request_refuse();
int popup_request_num_limit(gpointer);
void audio_request_accept();
void video_request_accept();