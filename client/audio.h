#pragma once

#define NET_DISCOVER_AUDIO 1
#define NET_DISCOVER_VIDEO 2
#define NET_DISCOVER_ONLINE_FILE 3


extern int deal_dicover_send_feedback(CRPBaseHeader *, u_int32_t );
extern int deal_audio_dicover_accept_feedback(CRPBaseHeader * );
extern void dealwith_request_audio_net_discover();
struct log_request_friend_discover{
    int uid;
    int requset_reason;
};
extern struct log_request_friend_discover the_log_request_friend_discover;
int  audio_request_refuse();
int popup_request_num_limit(gpointer);
int  audio_request_accept();
int  video_request_accept();

gboolean treatment_request_audio_discover(gpointer );
gboolean treatment_request_video_discover(gpointer );
int deal_video_dicover_server_feedback(CRPBaseHeader *, u_int32_t );
int deal_video_feedback(CRPBaseHeader *, u_int32_t );
int deal_audio_feedback(CRPBaseHeader *, u_int32_t );