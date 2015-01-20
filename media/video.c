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
#include <pwd.h>
#include "yuv422_rgb.h"
#include "video.h"
#include "logger.h"

#define SERVERPORT 5555

static struct sockaddr_in addr_opposite;

typedef struct VideoBuffer
{
    void *start;
    size_t length;
} VideoBuffer;


static VideoBuffer *buffers = NULL;
static unsigned char *rgbBuf;

//摄像头的文件描述符
static int fd;

//获取视频信息，发送视频信息，接受视频信息分别三个线程id
static pthread_t tid1, tid2, tid3;

void *primary_video(struct sockaddr_in *addr);

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

//接收到对面发来的关闭连接的信号
struct sigaction act;

//绘制视频的窗口
static GtkWindow *window;

static int flag_idle; //用于取消idle的旗帜
static int flag_main_idle;

//static int len_buffer;

void closewindow();

//用来更新视频标志位的函数指针
static int (*update_video_flag)();


int mark()
{
    int ret;
    struct v4l2_capability cap;
    struct v4l2_format fmt;

//    do
//    {
//        //查询视频设备的能力
//        ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
//    } while (ret == -1 && errno == EAGAIN);

    //这一小段用来判断摄像头能否正常使用。
    ret= ioctl(fd,VIDIOC_QUERYCAP,&cap);

    if(ret<0){
        printf("摄像头无法正常使用\n");
        pre_closewindow();
        return -1;
    }

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
        buf.index = (__u32) numBufs;
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
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

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
        p_send->jpeglen = (int) jpegWrite(tempbuf, (unsigned char *) p_send->jpeg_buf);
        free(tempbuf);
        tempbuf = NULL;
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

void video_off()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //停止视频采集
    int ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if (ret == -1)
    {
        printf("vidio OFF error!\n");
    }

    close(fd);
}

void cancle_mem()
{
    int i;
    for (i = 0; i < 4; i++)
    {
        munmap(buffers[i].start, 640 * 480 * 2);
    }
}

gint delete_event(GtkWindow *window)
{
    closewindow();
    //popup("消息","视频已结束");
    return FALSE;
}


void *pthread_video(void *arg)
{
    video_on();
    int times_err = 0;
    while (1)
    {
        if (video() == -1)
        {
            times_err++;
        }
        //视频采集出现三次错误就关掉聊天窗口
        if (times_err == 3) break;
        video_off();
        mark();
        cancle_mem();
        localMem();
        video_on();

    }
    //closewindow();
    g_idle_add(delete_event, NULL);
    return NULL;
}


void *pthread_snd(void *socketsd)
{

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
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
        pthread_cleanup_push(free, q_send);
                errno = 0;
                send(sd, &q_send->jpeglen, sizeof(int), MSG_MORE);
                send(sd, q_send->jpeg_buf, (size_t) q_send->jpeglen, 0);
        pthread_cleanup_pop(1);
        q_send = NULL;
        ///////////////////////////////////////////////////////////////////////////////////////////
    }
    return NULL;
}




void pre_closewindow(){
    g_idle_add((GSourceFunc)delete_event, NULL);
}

void *pthread_rev(void *socketrev)
{

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int sd = (*(int *) socketrev);
    jpeg_str *p_recv;
    ssize_t ret;


    while (1)
    {
        //////////////////////////////////////接受的循环队列/////////////////////////////////////////
        p_recv = (jpeg_str *) malloc(50000);
        errno = 0;
        ret = recv(sd, &p_recv->jpeglen, sizeof(int), MSG_WAITALL);
        //如果收到错误信号，就关闭窗口
        if (ret <= 0)
        {
            perror("recv");
            //delete_event();
            g_idle_add((GSourceFunc)delete_event, NULL);
            return NULL;
        }
        errno = 0;
        ret = recv(sd, p_recv->jpeg_buf, (size_t) p_recv->jpeglen, MSG_WAITALL);
        //如果收到错误信号，就关闭窗口
        if (ret <= 0)
        {
            perror("recv");
            g_idle_add((GSourceFunc)delete_event, NULL);
            return NULL;
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
    if (flag_idle) return 0;
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
    if (read_JPEG_file(q_recv->jpeg_buf, (char *) rgbBuf, (size_t) q_recv->jpeglen))
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(rgbBuf, GDK_COLORSPACE_RGB, 0, 8, 640, 480, 640 * 3, NULL, NULL);
        GtkImage *image = (GtkImage *) data;
        gtk_image_set_from_pixbuf(image, pixbuf);
        g_object_unref(pixbuf);
    }
    free(q_recv);
    q_recv = NULL;
    ///////////////////////////////////////////////////////////////////////////////////////////
    return 1;
}

//关闭窗口之后的处理函数
void closewindow()
{
//    pthread_detach(tid1);
//    pthread_detach(tid2);
//    pthread_detach(tid3);

    log_info("CloseWind", "Window is closing.\n");
    if(flag_idle==1) return ;
    flag_idle = 1;
    flag_main_idle = 1;

    //log_info("DEBUG", "cancel1");
    pthread_cancel(tid1);
    //log_info("DEBUG", "join1");
    pthread_join(tid1, NULL);
    //log_info("DEBUG", "cancel2");
    pthread_cancel(tid2);
    //log_info("DEBUG", "join2");
    pthread_join(tid2, NULL);
    //log_info("DEBUG", "cancel3");
    pthread_cancel(tid3);
    //log_info("DEBUG", "join3");
    pthread_join(tid3, NULL);
    //log_info("DEBUG", "cancel memory");

    cancle_mem();
    //log_info("DEBUG", "destory locks");

    pthread_mutex_unlock(&mutex_recv);
    pthread_mutex_destroy(&mutex_recv);
    pthread_mutex_unlock(&mutex_send);
    pthread_mutex_destroy(&mutex_send);

    pthread_cond_broadcast(&send_busy);
    pthread_cond_destroy(&send_busy);
    pthread_cond_broadcast(&send_idle);
    pthread_cond_destroy(&send_idle);
    pthread_cond_broadcast(&recv_busy);
    pthread_cond_destroy(&recv_busy);
    pthread_cond_broadcast(&recv_idle);
    pthread_cond_destroy(&recv_idle);
    //log_info("DEBUG", "close videos");

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //停止视频采集
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    //释放缓冲区，关闭设备文件
    close(fd);

    //log_info("DEBUG", "clean buffer");
    int i;
    for (i = 0; i < 8; i++)
    {
        free(circle_buf_recv[i]);
        circle_buf_recv[i] = NULL;
        free(circle_buf_send[i]);
        circle_buf_send[i] = NULL;
    }
//    free(*tail_recv);
//    (*tail_recv)=NULL;
//    free(*tail_send);
//    (*tail_send)=NULL;
    //log_info("DEBUG", "destory window");

    gtk_widget_destroy(GTK_WIDGET(window));
    //gtk_window_get_destroy_with_parent(window);

    //log_info("DEBUG", "free buf");
    //close(netSocket);

    //退出置前将标志位置为0;
    //FlagVideo=0;
    update_video_flag();
    free(rgbBuf);
}

//用于绘制视频窗口的入口
int guiMain(void *button)
{
    if (flag_main_idle) return 0;
    flag_idle = 0;
    rgbBuf = (unsigned char *) malloc(640 * 480 * 4);
    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    //logo
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
    GtkImage *image = GTK_IMAGE(gtk_image_new());
    gtk_widget_set_size_request(GTK_WIDGET(window), 640, 480);

    //char path_icon[80] = "";
    //sprintf(path_icon, "%s/.momo/theme/images/视频1.png", getpwuid(getuid())->pw_dir);//获取本机主题目录
    //gtk_window_set_icon(GTK_WINDOW(window), gdk_pixbuf_new_from_file(path_icon, NULL));//设置聊天窗口图标
    g_idle_add(idleDraw, image);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(image));
    gtk_widget_show_all(GTK_WIDGET(window));
    return 0;
}

void StartVideoChat(struct sockaddr_in *addr,int (*update_flag)()){
    update_video_flag=update_flag;
    pthread_t pthd_video_recv;
    pthread_create(&pthd_video_recv, NULL, primary_video, addr);
    pthread_join(&pthd_video_recv, NULL);
}

//视频聊天的函数入口
void *primary_video(struct sockaddr_in *addr)
{

    //用于取消idle的环境变量
    flag_main_idle = 0;

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
    //两个视频聊天的进程接受到的参数不一样。一方拿到对方的ip，一方参数为空
    //根据收到的参数判断是先连接对方，还是先监听对方
    if (addr != NULL)
    {
        netSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (netSocket == -1)
        {
            perror("socket\n");
            goto ERR;
        }
        setsockopt(netSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(int));

        //ret = inet_pton(AF_INET, argv, &addr_opposite.sin_addr);
        addr_opposite = *addr;
        if (connect(netSocket, (struct sockaddr *) &addr_opposite, sizeof(addr_opposite)) == -1)
        {
            perror("connect");
            close(netSocket);
            goto ERR;
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
            goto ERR;
        }
        ret = listen(listener, 1);
        if (ret == -1)
        {
            perror("listen\n");
            goto ERR;
        }


        if ((netSocket = accept(listener, (struct sockaddr *) &addr_opposite, &addrlen)) == -1)
        {
            perror("accept");
            close(listener);
            goto ERR;
        }
        close(listener);
    }

    //收到对面关闭连接的信号之后就关闭窗口
    act.sa_handler = pre_closewindow;
    if (sigaction(SIGPIPE, &act, NULL) == -1)
    {
        perror("sign error");
        goto ERR;
    }


    fd = open("/dev/video0", O_RDWR, 0);
    if (fd == -1)
    {
        perror("无法打开摄像头文件");
        //pre_closewindow();
        close(fd);
        goto ERR;
    }


    mark();
    localMem();
    pthread_create(&tid1, NULL, pthread_video, NULL);
    g_idle_add(guiMain, NULL);
    pthread_create(&tid3, NULL, pthread_rev, &netSocket);
    pthread_create(&tid2, NULL, pthread_snd, &netSocket);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    ERR:
    close(netSocket);
    return 0;
}

/*

int main(int argc,char**argv){
    //gtk_init(&argc, &argv);
    if(argc==1)primary_video(1,NULL);
    if(argc==2)primary_video(2,argv[1]);
}*/
