#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/videodev2.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "yuv422_rgb.h"
#include "video.h"

#define SERVERPORT 5555

struct sockaddr_in addr_opposite;

typedef struct VideoBuffer
{
    void *start;
    size_t length;
} VideoBuffer;


static VideoBuffer *buffers = NULL;
unsigned char *jpegbuf_my, *jpegbuf_opposite;
unsigned char *rgbBuf;
unsigned long jpeg_size_my;

int fd;

pthread_t tid1, tid2, tid3;


int is_jpeg_error = 1;

int read_JPEG_file(char *buf1, char *buf2);

//////////////////////循环队列/////////////////////////
static pthread_mutex_t mutex_send, mutex_recv;
static pthread_cond_t send_busy, send_idle, recv_busy, recv_idle;
typedef struct
{
    int jpeglen;
    char jpeg_buf[0];
} jpeg_str;
static jpeg_str *circle_buf_send[8], *circle_buf_recv[8];
static jpeg_str **head_send, **tail_send, **head_recv, **tail_recv;
//////////////////////////////////////////////////////


int mark()
{
    int ret;
    struct v4l2_capability cap;//»ñÈ¡ÊÓÆµÉè±žµÄ¹ŠÄÜ
    struct v4l2_format fmt;

    do
    {
        ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    } while (ret == -1 && errno == EAGAIN);

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("capability is V4L2_CAP_VIDEO_CAPTURE\n");
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//ÊýŸÝÁ÷ÀàÐÍ
    fmt.fmt.pix.width = 640;//¿í£¬±ØÐëÊÇ16µÄ±¶Êý
    fmt.fmt.pix.height = 480;//žß£¬±ØÐëÊÇ16µÄ±¶Êý
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;// ÊÓÆµÊýŸÝŽæŽ¢ÀàÐÍ
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0)
    {
        printf("set format failed\n");
        return -1;
    }

}

//ÉêÇëÎïÀíÄÚŽæ
int localMem()
{
    int numBufs = 0;
    struct v4l2_requestbuffers req;//·ÖÅäÄÚŽæ
    struct v4l2_buffer buf;
    ////////////////////////////////////////////////////////////////////////
    //这个req.count的大小是多少呢
    /////////////////////////////////////////////////////////////////////////
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    buffers = (VideoBuffer *) calloc(req.count, sizeof(VideoBuffer));

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
        return -1;
    }

    for (numBufs = 0; numBufs < req.count; numBufs++)
    {
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = numBufs;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)//¶ÁÈ¡»ºŽæÐÅÏ¢
        {
            printf("VIDIOC_QUERYBUF error\n");
            return -1;
        }
        buffers[numBufs].length = buf.length;
        buffers[numBufs].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd, buf.m.offset);//×ª»»³ÉÏà¶ÔµØÖ·
        if (buffers[numBufs].start == MAP_FAILED)
        {
            return -1;
        }
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)//·ÅÈë»ºŽæ¶ÓÁÐ
        {
            return -1;
        }
    }
}

void video_on()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
    {
        printf("VIDIOC_STREAMON error\n");
        // return -1;
    }
}

int video()
{
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;


    jpeg_str *p_send;
    while (1)
    {
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
        {
            printf("ioctl未知情况\n");
            return -1;
        }
        /////////////////////这里将yuv转化为jpeg///////////////////////////////////////////////////////
        unsigned char *tempbuf = (unsigned char *) malloc(640 * 480 * 3);
        yuv422_rgb24(buffers[buf.index].start, tempbuf, 640, 480);   //tempbuf是用来保存yuv转化为rgb的数据

        //pthread_mutex_lock(&g_lock_send);
        //jpeg_size_my = jpegWrite(tempbuf, jpegbuf_my);            //这里把tempbuf中的数据转化为jpeg数据
        //pthread_cond_signal(&g_cond_send);
        //pthread_mutex_unlock(&g_lock_send);
        //free(tempbuf);
        p_send = (jpeg_str *) malloc(50000);
        p_send->jpeglen = (int) jpegWrite(tempbuf, p_send->jpeg_buf);
        /////////////////////////////////////////采集循环队列/////////////////////////////////////////／
        //这里用来给p_send写好帧数据
        //memcpy(p_send->jpeg_buf, tempbuf, jpeg_size_my);
        //snd_pcm_readi(record.handle, p_send, 1000);


        pthread_mutex_lock(&mutex_send);
        while (*head_send) pthread_cond_wait(&send_idle, &mutex_send);
        *head_send = p_send;
        head_send = circle_buf_send + (head_send - circle_buf_send + 1) % (sizeof(circle_buf_send) / sizeof(*circle_buf_send));
        pthread_cond_signal(&send_busy);
        pthread_mutex_unlock(&mutex_send);
        /////////////////////////////////////////////////////////////////////////////////////////////
    }
    return 0;
}

void *pthread_video(void *arg)
{
    pthread_detach(pthread_self());
    video_on();
    /////////////////////
    while (1)
    {
        video();
    }
    /////////////////////
    return NULL;
}


void *pthread_snd(void *socketsd)
{
    pthread_detach(pthread_self());
    int sd = (*(int *) socketsd);
    int ret;
    jpeg_str *q_send;
    while (1)
    {
        //pthread_mutex_lock(&g_lock_send);
        //pthread_cond_wait(&g_cond_send, &g_lock_send);
        //send(sd, &(jpeg_size_my), sizeof(unsigned long), 0);
        //perror("send1");
        //send(sd, jpegbuf_my, jpeg_size_my, O_NONBLOCK);
        //perror("send2");
        //if (ret == -1)
        // {
        //   printf("client is out\n");
        //}
        //pthread_mutex_unlock(&g_lock_send);

        //////////////////////////////////发送的循环队列/////////////////////////////////////////////
        pthread_mutex_lock(&mutex_send);
        while (!(*tail_send))pthread_cond_wait(&send_busy, &mutex_send);
        q_send = *tail_send;
        *tail_send = NULL;
        tail_send = circle_buf_send + (tail_send - circle_buf_send + 1) % (sizeof(circle_buf_send) / sizeof(*circle_buf_send));
        pthread_cond_signal(&send_idle);
        pthread_mutex_unlock(&mutex_send);

        errno = 0;
        send(sd, &q_send->jpeglen, sizeof(int), MSG_MORE);
        fprintf(stderr, "fd:%d,err:%s\n", sd, strerror(errno));
        send(sd, q_send->jpeg_buf, q_send->jpeglen, 0);
        fprintf(stderr, "fd:%d,err:%s\n", sd, strerror(errno));

        free(q_send);
        ///////////////////////////////////////////////////////////////////////////////////////////
    }
    return NULL;
}


void *pthread_rev(void *socketrev)
{
    pthread_detach(pthread_self());
    int sd = (*(int *) socketrev);
    jpeg_str *p_recv;
    while (1)
    {
        //pthread_mutex_lock(&g_lock_recv);
        //recv(sd, &(jpeg_size_opposite), sizeof(uint64_t), 0);
        //perror("recv1");
        //perror("recv2");
        //recv(sd, jpegbuf_opposite, jpeg_size_opposite, MSG_WAITALL);

        //pthread_cond_signal(&g_cond_recv);
        //pthread_mutex_unlock(&g_lock_recv);
        //////////////////////////////////////接受的循环队列/////////////////////////////////////////
        p_recv = (struct jpeg_str *) malloc(50000);
        errno = 0;
        recv(sd, &p_recv->jpeglen, sizeof(int), MSG_WAITALL);
        perror("recv");
        errno = 0;
        recv(sd, p_recv->jpeg_buf, (size_t) p_recv->jpeglen, MSG_WAITALL);
        perror("recv");

        pthread_mutex_lock(&mutex_recv);
        while (*head_recv)
        {
            pthread_cond_wait(&recv_idle, &mutex_recv);
        }
        *head_recv = p_recv;
        head_recv = circle_buf_recv + (head_recv - circle_buf_recv + 1) % (sizeof(circle_buf_recv) / sizeof(*circle_buf_recv));
        pthread_cond_signal(&recv_busy);
        pthread_mutex_unlock(&mutex_recv);
        ///////////////////////////////////////////////////////////////////////////////////////////
    }
    return NULL;
}


gboolean idleDraw(gpointer data)
{
    jpeg_str *q_recv;
    pthread_mutex_lock(&mutex_recv);
    if (!(*tail_recv))
    {
        pthread_mutex_unlock(&mutex_recv);
        return 1;
    }
    //while (!(*tail_recv)) pthread_cond_wait(&recv_busy, &mutex_recv);
    q_recv = *tail_recv;
    *tail_recv = NULL;
    tail_recv = circle_buf_recv + (tail_recv - circle_buf_recv + 1) % (sizeof(circle_buf_recv) / sizeof(*circle_buf_recv));
    pthread_cond_signal(&recv_idle);
    pthread_mutex_unlock(&mutex_recv);

    //read_JPEG_file(q_recv.jpeg_buf, rgbBuf);
    if (read_JPEG_file(q_recv->jpeg_buf, rgbBuf))
    {

        ////////////////////////////////////////////////////////////////////////////////////////////
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(rgbBuf, GDK_COLORSPACE_RGB, 0, 8, 640, 480, 640 * 3, NULL, NULL);
        GtkImage *image = (GtkImage *) data;
        gtk_image_set_from_pixbuf(image, pixbuf);
        g_object_unref(pixbuf);
    }
    //playback.data_buf = q_recv;
    //SNDWAV_WritePcm(&playback, 1000);
    //snd_pcm_writei(playback.handle, q_recv, 1000);
    //SNDWAV_WritePcm(&playback, 1000);
    free(q_recv);
    ///////////////////////////////////////////////////////////////////////////////////////////
    return 1;
}

gint delete_event()
{
    gtk_main_quit();
    pthread_cancel(tid1);
    pthread_cancel(tid2);
    pthread_cancel(tid3);
    free(rgbBuf);
    return FALSE;
}

int guiMain(void *button)
{
    rgbBuf = (unsigned char *) malloc(640 * 480 * 4);
    GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
    GtkImage *image = GTK_IMAGE(gtk_image_new());
    gtk_widget_set_size_request(GTK_WIDGET(window), 640, 480);
    g_idle_add(idleDraw, image);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(image));
    gtk_widget_show_all(GTK_WIDGET(window));
    return 0;
}

void *primary_video(struct sockaddr_in *addr)
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
    signal(SIGPIPE, SIG_IGN);
    //////////////////////////////////////////////////////////////////
    int ret;
    //databuf_opposite = (buf_t *) malloc(640*480*4);
    //jpegbuf =(JPEG_t *)malloc(640*480*4);
    jpegbuf_my = (unsigned char *) malloc(50000);
    jpegbuf_opposite = (unsigned char *) malloc(50000);
    //////////////////////////////////////////////////////////////////
    ////////////////////////////这里设置发送套接字//////////////////////
    /*连接设置是指先发送数据还是先接受数据*/


    //struct sockaddr_in addr_opposite;
    socklen_t addrlen;
    addr_opposite.sin_family = AF_INET;
    addr_opposite.sin_port = htons(SERVERPORT);

    addrlen = sizeof(struct sockaddr_in);
    int netSocket;
    int on = 1;
    if (addr != NULL)
    {
        netSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (netSocket == -1)
        {
            perror("socket\n");
            return -1;
        }setsockopt(netSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(int));

        //ret = inet_pton(AF_INET, argv, &addr_opposite.sin_addr);
        addr_opposite = *addr;
        if (connect(netSocket, (struct sockaddr *) &addr_opposite, sizeof(addr_opposite)) == -1)
        {
            perror("connect");
            close(netSocket);
            return 1;
        }
    }
    else
    {
        struct sockaddr_in addr_my;
        bzero(&addr_my.sin_zero, sizeof(struct sockaddr));
        addr_my.sin_family = AF_INET;
        addr_my.sin_addr.s_addr = htons(INADDR_ANY);
        addr_my.sin_port = htons(SERVERPORT);
        int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        ret = bind(listener, (struct sockaddr *) &addr_my, sizeof(struct sockaddr_in));
        if (ret == -1)
        {
            perror("bind");
            exit(1);
        }
        ret = listen(listener, 1);
        if (ret == -1)
        {
            perror("listen\n");
            exit(1);
        }


        if ((netSocket = accept(listener, (struct sockaddr *) &addr_opposite, &addrlen)) == -1)
        {
            perror("accept");
            close(listener);
            return 1;
        }
        close(listener);
    }
    fd = open("/dev/video0", O_RDWR, 0);
    if (fd == -1)
    {
        perror("open");
        close(fd);
        return 0;
    }


    mark();
    localMem();
    ret = pthread_create(&tid1, NULL, pthread_video, NULL);
    g_idle_add(guiMain, NULL);
    ret = pthread_create(&tid3, NULL, pthread_rev, &netSocket);
    ret = pthread_create(&tid2, NULL, pthread_snd, &netSocket);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    close(netSocket);
    free(jpegbuf_my);
    free(jpegbuf_opposite);
    return 0;
}

/*

int main(int argc,char**argv){
    //gtk_init(&argc, &argv);
    if(argc==1)primary_video(1,NULL);
    if(argc==2)primary_video(2,argv[1]);
}*/
