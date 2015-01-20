#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include "audio.h"
#include "../logger/include/logger.h"

static pthread_t mainThread;
static int netSocket;

static struct sockaddr_in addr_sendto;
static socklen_t addr_sendto_len;

/*循环队列甬道的全局变量*/
static pthread_mutex_t mutex_send, mutex_recv;
static pthread_cond_t send_busy, send_idle, recv_busy, recv_idle;
static char *circle_buf_send[8], *circle_buf_recv[8];
static char **head_send, **tail_send, **head_recv, **tail_recv;

static int (*cleanupFunc)(void *);

static void *cleanupArg;

void *record_routine(void *data)
{
    SNDPCMContainer_t record;
    WAVContainer_t wav;
    /*将要录制的音频格式设置好*/
    char bincode[] = {
            0x52, 0x49, 0x46, 0x46, 0x64, 0x1f, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
            0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1f, 0x00, 0x00, 0x40, 0x1f, 0x00, 0x00,
            0x10, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x40, 0x1f, 0x00, 0x00
    };
    /*将设置好的数据导入wav*/
    memcpy(&wav, &bincode, 44);
    //////////////////////////////////////////////////////////////////////////////////
    //创造一个新的输出对象的指针h
    if (snd_output_stdio_attach(&record.log, stderr, 0) < 0)
    {
        fprintf(stderr, "Error snd_output_stdio_attach\n");
        goto Err;
    }
    //打开一路pcm。刷新config配置
    if (snd_pcm_open(&record.handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0)
    {
        fprintf(stderr, "Error snd_pcm_open [ default]\n");
        goto Err;
    }
    pthread_cleanup_push(snd_pcm_close, &record.handle);
            //设置参数
            if (SNDWAV_SetParams(&record, &wav) < 0)
            {
                fprintf(stderr, "Error set_snd_pcm_params\n");
                goto Err;
            }
            //sendtoAssigned = 0;
            char *p_send;
            /*循环录制自己的音频数据*/
            while (1)
            {
                p_send = (char *) malloc(1000);
                snd_pcm_readi(record.handle, p_send, 1000);
                pthread_mutex_lock(&mutex_send);
                while (*head_send) pthread_cond_wait(&send_idle, &mutex_send);
                *head_send = p_send;
                head_send = circle_buf_send + (head_send - circle_buf_send + 1) % (sizeof(circle_buf_send) / sizeof(*circle_buf_send));
                pthread_cond_signal(&send_busy);
                pthread_mutex_unlock(&mutex_send);
            }
    pthread_cleanup_pop(1);
    Err:
    return 0;
}

void *send_routine(void *data)
{
    char *q_send;
    /*循环发送自己录制好的音频数据*/
    while (1)
    {
        pthread_mutex_lock(&mutex_send);
        while (!(*tail_send))pthread_cond_wait(&send_busy, &mutex_send);
        q_send = *tail_send;
        *tail_send = NULL;
        tail_send = circle_buf_send + (tail_send - circle_buf_send + 1) % (sizeof(circle_buf_send) / sizeof(*circle_buf_send));
        pthread_cond_signal(&send_idle);
        pthread_mutex_unlock(&mutex_send);
        sendto(netSocket, q_send, 1000, 0, (struct sockaddr *) &addr_sendto, sizeof(struct sockaddr_in));
        free(q_send);
    }
}

void *recv_routine(void *data)
{
    char *p_recv;
    /*用来循环接受对方发来的音频数据*/
    while (1)
    {
        p_recv = (char *) malloc(1000);
        recvfrom(netSocket, p_recv, 1000, 0, &addr_sendto, &addr_sendto_len);
        pthread_mutex_lock(&mutex_recv);
        while (*head_recv)
        {
            pthread_cond_wait(&recv_idle, &mutex_recv);
        }
        *head_recv = p_recv;
        head_recv = circle_buf_recv + (head_recv - circle_buf_recv + 1) % (sizeof(circle_buf_recv) / sizeof(*circle_buf_recv));
        pthread_cond_signal(&recv_busy);
        pthread_mutex_unlock(&mutex_recv);
    }
}

void *play_routine(void *data)
{
    SNDPCMContainer_t playback;
    WAVContainer_t wav;
    char bincode[] = {
            0x52, 0x49, 0x46, 0x46, 0x64, 0x1f, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
            0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1f, 0x00, 0x00, 0x40, 0x1f, 0x00, 0x00,
            0x10, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x40, 0x1f, 0x00, 0x00
    };
    memcpy(&wav, &bincode, 44);
    if (snd_output_stdio_attach(&playback.log, stderr, 0) < 0)
    {
        fprintf(stderr, "Error snd_output_stdio_attach\n");
        goto Err;
    }
    //打开一路pcm。刷新config配置
    if (snd_pcm_open(&playback.handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
    {
        fprintf(stderr, "Error snd_pcm_open [ default ]\n");
        goto Err;
    }
    pthread_cleanup_push(snd_pcm_close, &playback.handle);
            //设置参数
            if (SNDWAV_SetParams(&playback, &wav) < 0)
            {
                fprintf(stderr, "Error set_snd_pcm_params\n");
                goto Err;
            }
            //socklen_t i = sizeof(struct sockaddr_in);
            char *q_recv;
            /*用来循环播音*/
            while (1)
            {
                pthread_mutex_lock(&mutex_recv);
                while (!(*tail_recv)) pthread_cond_wait(&recv_busy, &mutex_recv);
                q_recv = *tail_recv;
                *tail_recv = NULL;
                pthread_cleanup_push(free, q_recv);
                        tail_recv = circle_buf_recv + (tail_recv - circle_buf_recv + 1) % (sizeof(circle_buf_recv) / sizeof(*circle_buf_recv));
                        pthread_cond_signal(&recv_idle);
                        pthread_mutex_unlock(&mutex_recv);
                        playback.data_buf = (uint8_t *) q_recv;
                        SNDWAV_WritePcm(&playback, 1000);
                        //snd_pcm_writei(playback.handle, q_recv, 1000);
                        //SNDWAV_WritePcm(&playback, 1000);
                pthread_cleanup_pop(1);
            }
    pthread_cleanup_pop(1);
    return 0;


    Err:
    if (playback.data_buf) free(playback.data_buf);
    if (playback.log) snd_output_close(playback.log);
    if (playback.handle) snd_pcm_close(playback.handle);
    return 0;
}

static int InitAudioChat()
{
    head_send = circle_buf_send;
    tail_send = circle_buf_send;
    head_recv = circle_buf_recv;
    tail_recv = circle_buf_recv;
    pthread_mutex_init(&mutex_send, 0); //记得摧毁锁
    pthread_cond_init(&send_idle, NULL);
    pthread_cond_init(&send_busy, NULL);
    pthread_mutex_init(&mutex_recv, 0); //记得摧毁锁
    pthread_cond_init(&recv_idle, NULL);
    pthread_cond_init(&recv_busy, NULL);
    return 1;
}

static void cleanupAudioChat(void *__noused)
{
    log_info("Audio Chat Cleanup", "Cleanup\n");
    pthread_mutex_unlock(&mutex_send);
    pthread_mutex_destroy(&mutex_send);
    pthread_cond_destroy(&send_idle);
    pthread_cond_destroy(&send_busy);
    pthread_mutex_unlock(&mutex_recv);
    pthread_mutex_destroy(&mutex_recv);
    pthread_cond_destroy(&recv_idle);
    pthread_cond_destroy(&recv_busy);
    close(netSocket);
}

static void *process(void *data)
{
    pthread_cleanup_push(cleanupFunc, cleanupArg);
            pthread_cleanup_push(cleanupAudioChat, 0);
                    pthread_t t_record, t_send, t_recv, t_play;
                    pthread_create(&t_send, NULL, send_routine, NULL);
                    pthread_create(&t_recv, NULL, recv_routine, NULL);
                    pthread_create(&t_record, NULL, record_routine, NULL);
                    pthread_create(&t_play, NULL, play_routine, NULL);
                    pthread_cleanup_push(pthread_cancel, t_send);
                            pthread_cleanup_push(pthread_cancel, t_recv);
                                    pthread_cleanup_push(pthread_cancel, t_record);
                                            pthread_cleanup_push(pthread_cancel, t_play);
                                                    pthread_join(t_send, NULL);
                                                    pthread_join(t_recv, NULL);
                                                    pthread_join(t_record, NULL);
                                                    pthread_join(t_play, NULL);
                                            pthread_cleanup_pop(1);
                                    pthread_cleanup_pop(1);
                            pthread_cleanup_pop(1);
                    pthread_cleanup_pop(1);
            pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return 0;
}

void StartAudioChat(int sendSock, struct sockaddr_in *addr, int (*onCancel)(void *), void *data)
{
    if (mainThread)
    {
        pthread_cancel(mainThread);
    }
    InitAudioChat();
    addr_sendto = *addr;
    netSocket = sendSock;
    pthread_create(&mainThread, NULL, process, NULL);
    cleanupFunc = onCancel;
    cleanupArg = data;
}

void StopAudioChat(void)
{
    pthread_cancel(mainThread);
    mainThread = 0;
}