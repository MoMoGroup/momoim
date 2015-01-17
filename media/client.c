#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <semaphore.h>
#include "sound.h"

char *devicename = "default";
WAVContainer_t wav;
SNDPCMContainer_t record;
SNDPCMContainer_t playback;
int sock_send, sock_recv;
struct sockaddr_in addr_my;
unsigned int sem_send_number = 0;
unsigned int sem_recv_number = 0;
sem_t sem_send;
sem_t sem_recv;

struct sockaddr_in addr_opposite;
int flag_send;//收到第一帧获得对面ip地址之后解开锁...如果这个文件被有对方地址的函数调用。这个flag_send=1

//////////////////////循环队列/////////////////////////
pthread_mutex_t mutex_send, mutex_recv;
pthread_cond_t send_busy, send_idle, recv_busy, recv_idle;
char *circle_buf_send[8], *circle_buf_recv[8];
char **head_send, **tail_send,**head_recv,**tail_recv;

//////////////////////////////////////////////////////
void *pthread_record() {
    //////////////////////////////////////////////////////////////////////////////////
    //创造一个新的输出对象的指针h
    if (snd_output_stdio_attach(&record.log, stderr, 0) < 0) {
        fprintf(stderr, "Error snd_output_stdio_attach\n");
        goto Err;
    }
    //打开一路pcm。刷新config配置
    if (snd_pcm_open(&record.handle, devicename, SND_PCM_STREAM_CAPTURE, 0) < 0) {
        fprintf(stderr, "Error snd_pcm_open [ %s]\n", devicename);
        goto Err;
    }
    //设置参数
    if (SNDWAV_SetParams(&record, &wav) < 0) {
        fprintf(stderr, "Error set_snd_pcm_params\n");
        goto Err;
    }
    //flag_send = 0;
    //head = (char *) malloc(sizeof(char *));
    //tail = (char *) malloc(sizeof(char *));
    ///////////////////////////////////////////////////////////////////////////////////
    char *p_send;
    while (1) {
        p_send = (char *) malloc(1000);
        snd_pcm_readi(record.handle, p_send, 1000);
        printf(".");
        pthread_mutex_lock(&mutex_send);
        while (*head_send) pthread_cond_wait(&send_idle, &mutex_send);
        *head_send = p_send;
        head_send = circle_buf_send + (head_send - circle_buf_send + 1) % (sizeof(circle_buf_send) / sizeof(*circle_buf_send));
        pthread_cond_signal(&send_busy);
        pthread_mutex_unlock(&mutex_send);
    }
    ///////////////////////////////////////////////////////////////////////////////////
    Err:
    return 0;
}

void *pthread_send(struct sockaddr_in *addr_opposite) {
    char *q_send;
    while (1) {
        pthread_mutex_lock(&mutex_send);
        while (!(*tail_send))pthread_cond_wait(&send_busy, &mutex_send);
        q_send = *tail_send;
        *tail_send = NULL;
        tail_send = circle_buf_send + (tail_send - circle_buf_send + 1) % (sizeof(circle_buf_send) / sizeof(*circle_buf_send));
        pthread_cond_signal(&send_idle);
        pthread_mutex_unlock(&mutex_send);
        sendto(sock_send, q_send, 1000, 0, (struct sockaddr *) addr_opposite, sizeof(struct sockaddr_in));
        free(q_send);
    }
}

void *pthread_recv() {
    char *p_recv;
    struct sockaddr *opposite_addr;
    socklen_t len_opposite_addr;
    while (1) {
        p_recv = (char*)malloc(1000);
        recvfrom(sock_recv, p_recv , 1000, 0, opposite_addr, &len_opposite_addr);
        pthread_mutex_lock(&mutex_recv);
        while(*head_recv) pthread_cond_wait(&recv_idle , &mutex_recv);
        *head_recv = p_recv;
        head_recv = circle_buf_recv + (head_recv  - circle_buf_recv + 1) % (sizeof(circle_buf_recv) / sizeof(*circle_buf_recv));
        pthread_cond_signal(&recv_busy);
        pthread_mutex_unlock(&mutex_recv);
        if(flag_send==0) {
            pthread_mutex_unlock(&mutex_send);
            flag_send++;
        }
    }
}

void*  primary_audio(struct sockaddr_in*addr) {
    /////////////////////////////////////////初始化本地地址//////////////////////////////////////////
    bzero(&addr_my.sin_zero, sizeof(struct sockaddr));//清空
    addr_my.sin_family = AF_INET;//初始化网络协议
    addr_my.sin_addr.s_addr = htonl(INADDR_ANY);//指定本地绑定网卡
    addr_my.sin_port = htons(7777);//指定绑定的监听端口
    sock_recv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//创建socket
    if (sock_recv < 0) {
        printf("socket error!\n");
        exit(1);
    }
    if (bind(sock_recv, (struct sockaddr *) &addr_my, sizeof(struct sockaddr_in)) == -1) {
        perror("bind error!\n");//绑定
        exit(1);
    }
    //////////////////////////////////////创建发送套接字/////////////////////////////////////////////////////////
    sock_send = socket(PF_INET, SOCK_DGRAM, 0);//创建socket
    if (sock_send < 0) {
        printf("socket error!\n");
       exit(1);
    }
    ///////////////////////////////////////数据预处理///////////////////////////////////////////////
    char bincode[] = {
            0x52, 0x49, 0x46, 0x46, 0x64, 0x1f, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
            0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1f, 0x00, 0x00, 0x40, 0x1f, 0x00, 0x00,
            0x10, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x40, 0x1f, 0x00, 0x00
    };
    memcpy(&wav, &bincode, 44);
    //record.data_buf = (uint8_t *) malloc(5000);
    //memset(record.data_buf, 0x0, sizeof(SNDPCMContainer_t));//清空record
    playback.data_buf = (uint8_t *) malloc(5000);
    memset(playback.data_buf, 0x0, sizeof(SNDPCMContainer_t));//清空playback
    ////////////////////////////////////////创建线程/////////////////////////////////////////////////
    head_send = circle_buf_send;
    tail_send = circle_buf_send;
    head_recv = circle_buf_recv;
    tail_recv = circle_buf_recv;
    pthread_t pthd_record, pthd_send, pthd_recv;
    pthread_mutex_init(&mutex_send, 0); //记得摧毁锁
    pthread_cond_init(&send_idle, NULL);
    pthread_cond_init(&send_busy, NULL);
    pthread_mutex_init(&mutex_recv, 0); //记得摧毁锁
    pthread_cond_init(&recv_idle, NULL);
    pthread_cond_init(&recv_busy, NULL);
    //因为没写一地址的方没有对面的IP地址。所以在收到对面的第一帧后才开始给对面发送数据。因此一开始锁住发送线程的send
    if(addr==NULL) {
        pthread_mutex_lock(&mutex_send);
        flag_send=0;
    }
    else{
        flag_send=1;
        //addr_opposite.sin_family = AF_INET;
        //addr_opposite.sin_port = htons(7777);
        //inet_pton(AF_INET,argv,&addr_opposite.sin_addr);
        addr_opposite=*addr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    pthread_create(&pthd_send, NULL, pthread_send,&addr_opposite);
    pthread_create(&pthd_recv, NULL, pthread_recv, NULL);
    pthread_create(&pthd_record, NULL, pthread_record, NULL);
    //////////////////////////////主函数用来播放对面的语言消息//////////////////////////////////////////
    //创造一个新的输出对象的指针h
    if (snd_output_stdio_attach(&playback.log, stderr, 0) < 0) {
        fprintf(stderr, "Error snd_output_stdio_attach\n");
        goto Err;
    }
    //打开一路pcm。刷新config配置
    if (snd_pcm_open(&playback.handle, devicename, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error snd_pcm_open [ %s]\n", devicename);
        goto Err;
    }
    //设置参数
    if (SNDWAV_SetParams(&playback, &wav) < 0) {
        fprintf(stderr, "Error set_snd_pcm_params\n");
        goto Err;
    }
    socklen_t i = sizeof(struct sockaddr_in);
    ///////////////////////////////////////////////////////////////////////////////////
    char *q_recv;
    while (1) {
        pthread_mutex_lock(&mutex_recv);
        while(!(*tail_recv)) pthread_cond_wait(&recv_busy , &mutex_recv);
        q_recv = *tail_recv;
        *tail_recv = NULL;
        tail_recv = circle_buf_recv + (tail_recv - circle_buf_recv + 1) % (sizeof(circle_buf_recv) / sizeof(*circle_buf_recv));
        pthread_cond_signal(&recv_idle);
        pthread_mutex_unlock(&mutex_recv);
        playback.data_buf=q_recv;
        SNDWAV_WritePcm(&playback, 1000);
        //snd_pcm_writei(playback.handle, q_recv, 1000);
        printf("*");
        //SNDWAV_WritePcm(&playback, 1000);
        free(q_recv);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    return 0;


    Err:
    if (record.data_buf) free(record.data_buf);
    if (record.log) snd_output_close(record.log);
    if (record.handle) snd_pcm_close(record.handle);
    return -1;
}


//int main(int argc,char**argv){
//    setbuf(stdout, 0);
//    setbuf(stderr, 0);
//
//    /////////////////////////////////////////初始化对面地址///////////////////////////////////////////////////////////
//    //if (argc < 2) {
//      //  printf("please input the other side addr\n");
//        //return 0;
//    //}
//    //bzero(&addr_opposite, sizeof(struct sockaddr));
//    //inet_pton(PF_INET, argv[1], &addr_opposite.sin_addr);
//    //addr_opposite.sin_port = htons(7777);
//    //sock_send = socket(PF_INET, SOCK_DGRAM, 0);//创建socket
////    if (sock_send < 0) {
////        printf("socket error!\n");
////        exit(1);
////    }
//    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    ///////////////////////////////////////////////修改中/////////////////////////////////////////////////////////////
//    //if(argc==1)primary_audio(1,NULL);
//    //if(argc==2)primary_audio(2,argv[1]);
//    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    primary_audio(1,NULL);
//    primary_audio(2, argv[1]);
//    return 0;
//}
