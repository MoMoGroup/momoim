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
unsigned char *rgbBuf;

int fd;//摄像头的文件描述符

pthread_t tid1, tid2, tid3;

/*下面的代码用来做循环队列*/
static pthread_mutex_t mutex_send, mutex_recv;
static pthread_cond_t send_busy, send_idle, recv_busy, recv_idle;
typedef struct
{
    int jpeglen;
    char jpeg_buf[0];
} jpeg_str;
static jpeg_str *circle_buf_send[8], *circle_buf_recv[8];
static jpeg_str **head_send, **tail_send, **head_recv, **tail_recv;

struct sigaction act;
GtkWindow *window;

void closewindow();

int mark()
{
    int ret;
    struct v4l2_capability cap;
    struct v4l2_format fmt;

    do
    {
        //查询视频设备的能力
        ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    } while (ret == -1 && errno == EAGAIN);

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("capability is V4L2_CAP_VIDEO_CAPTURE\n");
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    //设置视频采集的参数
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0)
    {
        printf("set format failed\n");
        return -1;
    }
    return 0;

}


int localMem()
{
    int numBufs = 0;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    buffers = (VideoBuffer *) calloc(req.count, sizeof(VideoBuffer));
    //申请若干个缓冲区。这里是四个
    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
        return -1;
    }

    for (numBufs = 0; numBufs < req.count; numBufs++)
    {
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = (__u32)numBufs;
        //查询缓冲区在内核空间中的长度和偏移量
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            printf("VIDIOC_QUERYBUF error\n");
            return -1;
        }
        buffers[numBufs].length = buf.length;
        buffers[numBufs].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd, buf.m.offset);
        if (buffers[numBufs].start == MAP_FAILED)
        {
            return -1;
        }
        //将申请到的帧缓冲区全部放入视频采集的输出队列
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
        {
            return -1;
        }
    }
    return 0;
}

void video_on()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //开始视频流数据采集
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
    {
        printf("VIDIOC_STREAMON error\n");
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
        //应用程序从视频采集输出队列中去除已含有采集数据的帧缓冲区
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
        {
            printf("ioctl出现错误\n");
            return -1;
        }
        unsigned char *tempbuf = (unsigned char *) malloc(640 * 480 * 3);
        /*tempbuf是用来保存yuv转化为rgb的数据*/
        yuv422_rgb24(buffers[buf.index].start, tempbuf, 640, 480);
        p_send = (jpeg_str *) malloc(50000);
        /*将tempbuf中的数据转换成jpeg放入p->send中*/
        p_send->jpeglen = (int) jpegWrite(tempbuf, p_send->jpeg_buf);
        free(tempbuf);
        //应用程序将该帧缓冲区重新排入输入队列
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
        {
            return -1;
        }
        /////////////////////////////////////////采集循环队列/////////////////////////////////////////／
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
    video_on();
    while (1)
    {
        video();
    }
    return NULL;
}


void *pthread_snd(void *socketsd)
{
    int sd = (*(int *) socketsd);
    jpeg_str *q_send;
    while (1)
    {
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
        send(sd, q_send->jpeg_buf, q_send->jpeglen, 0);

        free(q_send);
        ///////////////////////////////////////////////////////////////////////////////////////////
    }
    return NULL;
}


void *pthread_rev(void *socketrev)
{
    int sd = (*(int *) socketrev);
    jpeg_str *p_recv;
    ssize_t ret;


    while (1)
    {
        //////////////////////////////////////接受的循环队列/////////////////////////////////////////
        p_recv = (jpeg_str *) malloc(50000);
        errno = 0;
        ret = recv(sd, &p_recv->jpeglen, sizeof(int), MSG_WAITALL);
        if (ret <= 0)
        {
            perror("recv");
            //delete_event();
            closewindow();
        }
        errno = 0;
        ret = recv(sd, p_recv->jpeg_buf, (size_t) p_recv->jpeglen, MSG_WAITALL);
        if(ret<=0){
            perror("recv");
            //delete_event();
            closewindow();
        };
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
    if (!*tail_recv)
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
    if (read_JPEG_file(q_recv->jpeg_buf, rgbBuf, (size_t) q_recv->jpeglen))
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(rgbBuf, GDK_COLORSPACE_RGB, 0, 8, 640, 480, 640 * 3, NULL, NULL);
        GtkImage *image = (GtkImage *) data;
        gtk_image_set_from_pixbuf(image, pixbuf);
        g_object_unref(pixbuf);
    }
    fprintf(stderr,".");
    free(q_recv);
    ///////////////////////////////////////////////////////////////////////////////////////////
    return 1;
}

void closewindow(){
    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF,&type);//停止视频采集
    close(fd);//释放缓冲区，关闭设备文件

    pthread_cancel(tid1);
    pthread_cancel(tid2);
    pthread_cancel(tid3);

    //gtk_widget_destroy(window);
    gtk_window_get_destroy_with_parent(window);



    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    free(rgbBuf);
}


gint delete_event(GtkWindow *window)
{
    //gtk_main_quit();
//    gtk_widget_destroy(window);
//    pthread_detach(tid1);
//    pthread_detach(tid2);
//    pthread_detach(tid3);
//    pthread_cancel(tid1);
//    pthread_join(tid1, NULL);
//    pthread_cancel(tid2);
//    pthread_join(tid2, NULL);
//    pthread_cancel(tid3);
//    pthread_join(tid3, NULL);
//    free(rgbBuf);
    closewindow();
    return FALSE;
}

int guiMain(void *button)
{
    rgbBuf = (unsigned char *) malloc(640 * 480 * 4);
    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
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
    /*设置四个环境变量和四个锁，用来做循环队列*/
    pthread_mutex_init(&mutex_send, 0);
    pthread_cond_init(&send_idle, NULL);
    pthread_cond_init(&send_busy, NULL);
    pthread_mutex_init(&mutex_recv, 0);
    pthread_cond_init(&recv_idle, NULL);
    pthread_cond_init(&recv_busy, NULL);
    signal(SIGPIPE, SIG_IGN);
    int ret;
    ////////////////////////////这里设置发送套接字//////////////////////


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
            return NULL;
        }
        setsockopt(netSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(int));

        //ret = inet_pton(AF_INET, argv, &addr_opposite.sin_addr);
        addr_opposite = *addr;
        if (connect(netSocket, (struct sockaddr *) &addr_opposite, sizeof(addr_opposite)) == -1)
        {
            perror("connect");
            close(netSocket);
            return NULL;
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
            return NULL;
        }
        close(listener);
    }

    act.sa_handler=closewindow;
    if(sigaction(SIGPIPE, &act , NULL)==-1)
        perror("sign error");


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
    return 0;
}

/*

int main(int argc,char**argv){
    //gtk_init(&argc, &argv);
    if(argc==1)primary_video(1,NULL);
    if(argc==2)primary_video(2,argv[1]);
}*/
