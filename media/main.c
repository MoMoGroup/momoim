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

#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "yuv422_rgb.h"
#include "video.h"

#define SERVERPORT 5555

struct sockaddr_in addr_opposite;

typedef struct VideoBuffer {
    void *start;
    size_t length;
} VideoBuffer;


static VideoBuffer *buffers = NULL;
unsigned char *jpegbuf_my, *jpegbuf_opposite;
unsigned char *rgbBuf;
unsigned long jpeg_size_my, jpeg_size_opposite;

pthread_mutex_t g_lock_send, g_lock_recv;
pthread_cond_t g_cond_send, g_cond_recv;
int fd;

pthread_t tid1, tid2, tid3;


int is_jpeg_error = 1;

int read_JPEG_file(char *buf1, char *buf2);

int mark() {
    int ret;
    struct v4l2_capability cap;//»ñÈ¡ÊÓÆµÉè±žµÄ¹ŠÄÜ
    struct v4l2_format fmt;

    do {
        ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    } while (ret == -1 && errno == EAGAIN);

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        printf("capability is V4L2_CAP_VIDEO_CAPTURE\n");
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//ÊýŸÝÁ÷ÀàÐÍ
    fmt.fmt.pix.width = 640;//¿í£¬±ØÐëÊÇ16µÄ±¶Êý
    fmt.fmt.pix.height = 480;//žß£¬±ØÐëÊÇ16µÄ±¶Êý
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;// ÊÓÆµÊýŸÝŽæŽ¢ÀàÐÍ
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        printf("set format failed\n");
        return -1;
    }

}

//ÉêÇëÎïÀíÄÚŽæ
int localMem() {
    int numBufs = 0;
    struct v4l2_requestbuffers req;//·ÖÅäÄÚŽæ
    struct v4l2_buffer buf;
    ////////////////////////////////////////////////////////////////////////
    //这个req.count的大小是多少呢
    buffers = (VideoBuffer *) calloc(req.count, sizeof(VideoBuffer));
    /////////////////////////////////////////////////////////////////////////
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        return -1;
    }

    for (numBufs = 0; numBufs < req.count; numBufs++) {
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
        if (buffers[numBufs].start == MAP_FAILED) {
            return -1;
        }
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)//·ÅÈë»ºŽæ¶ÓÁÐ
        {
            return -1;
        }
    }
}

void video_on() {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        printf("VIDIOC_STREAMON error\n");
        // return -1;
    }
}

int video() {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    while (1) {
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            return -1;
        }
        /////////////////////这里将yuv转化为jpeg///////////////////////////////////////////////////////
        unsigned char *tempbuf = (unsigned char *) malloc(640 * 480 * 3);
        yuv422_rgb24(buffers[buf.index].start, tempbuf, 640, 480);   //tempbuf是用来保存yuv转化为rgb的数据
        jpeg_size_my = jpegWrite(tempbuf, jpegbuf_my);            //这里把tempbuf中的数据转化为jpeg数据
        free(tempbuf);
        //FILE *fp = fopen("/tmp/2.jpg", "w");
        //fwrite(jpegbuf, jpeg_size, 1, fp);
        //fclose(fp);
        //////////////////////////////////////////////////////////////////
        pthread_cond_signal(&g_cond_send);
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            return -1;
        }
    }
    return 0;
}

void *pthread_video(void *arg) {
    pthread_detach(pthread_self());
    video_on();
    /////////////////////
    while (1) {
        video();
    }
    /////////////////////
    return NULL;
}



void *pthread_snd(void *socketsd) {
    pthread_detach(pthread_self());
    int sd = (*(int *) socketsd);
    int ret;
    while (1) {
        pthread_mutex_lock(&g_lock_send);
        pthread_cond_wait(&g_cond_send, &g_lock_send);
        send(sd, &(jpeg_size_my), sizeof(unsigned long), 0);
        //perror("send1");
        send(sd, jpegbuf_my, jpeg_size_my, O_NONBLOCK);
        //perror("send2");
        if (ret == -1) {
            printf("client is out\n");
        }
        pthread_mutex_unlock(&g_lock_send);
    }
    return NULL;
}


void *pthread_rev(void *socketrev) {
    pthread_detach(pthread_self());
    int sd = (*(int *) socketrev);
    while (1) {
        recv(sd, &(jpeg_size_opposite), sizeof(unsigned long), 0);
        //perror("recv1");
        //perror("recv2");
        recv(sd, jpegbuf_opposite, jpeg_size_opposite, MSG_WAITALL);

        pthread_cond_signal(&g_cond_recv);
    }
    return NULL;
}


gboolean idleDraw(gpointer data) {

    pthread_mutex_lock(&g_lock_recv);
    pthread_cond_wait(&g_cond_recv, &g_lock_recv);
    //////////////////////////////在这里将jpeg数据转化为rgb数据////////////////////////////////////

    if (read_JPEG_file(jpegbuf_opposite, rgbBuf)) {

        ////////////////////////////////////////////////////////////////////////////////////////////
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(rgbBuf, GDK_COLORSPACE_RGB, 0, 8, 640, 480, 640 * 3, NULL, NULL);
        GtkImage *image = (GtkImage *) data;
        gtk_image_set_from_pixbuf(image, pixbuf);
        g_object_unref(pixbuf);
    }
    pthread_mutex_unlock(&g_lock_recv);
    return 1;
}

gint delete_event() {
    gtk_main_quit();
    pthread_cancel(tid1);
    pthread_cancel(tid2);
    pthread_cancel(tid3);
    return FALSE;
}

void *guiMain(void *button) {
    pthread_detach(pthread_self());
    rgbBuf = (unsigned char *) malloc(640 * 480 * 4);
    GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
    GtkImage *image = GTK_IMAGE(gtk_image_new());
    gtk_widget_set_size_request(GTK_WIDGET(window), 640, 480);
    g_idle_add(idleDraw, image);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(image));
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();
    free(rgbBuf);
    return 0;
}

int primary_video(int argc,char *argv) {

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

    //addrlen = sizeof(struct sockaddr_in);
    int sock_send = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_send == -1) {
        perror("socket\n");
        exit(1);
    }
    //////////////////////////////////////////////////////////////////

    ///////////////////////这里设置接受套接字///////////////////////////
    struct sockaddr_in addr_my;
    bzero(&addr_my.sin_zero, sizeof(struct sockaddr));
    addr_my.sin_family = AF_INET;
    addr_my.sin_addr.s_addr = htons(INADDR_ANY);
    addr_my.sin_port = htons(SERVERPORT);
    int sock_recv = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_recv == -1) {
        perror("recv socket\n");
        exit(1);
    }
    ret = bind(sock_recv, (struct sockaddr *) &addr_my, sizeof(struct sockaddr_in));
    if (ret == -1) {
        perror("bind");
        exit(1);
    }
    ret = listen(sock_recv, 1);
    if (ret == -1) {
        perror("listen\n");
        exit(1);
    }
    int on = 1;
    setsockopt(sock_recv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
//    ret = bind(sock_recv, (struct sockaddr *) &addr_my, sizeof(struct sockaddr_in));
//    if (ret == -1) {
//        perror("bind");
//        exit(1);
//    }
//    ret = listen(sock_recv, 1);
//    if (ret == -1) {
//        perror("listen\n");
//        exit(1);
//    }
    //////////////////////////////////////////////////////////////////
    ///////////////////////这里是连接设置的部分//////////////////////////
    /*根据参数写不写地址分成两个不同的进程，写地址的尝试连接对方的进程，不写地址的进程等待对方连接*/
    int newsd;
    if (argc >= 2) {
        ret = inet_pton(AF_INET, argv, &addr_opposite.sin_addr);

        if (connect(sock_send, (struct sockaddr *) &addr_opposite, sizeof(addr_opposite)) == -1) {
            perror("connect");
            close(sock_send);
            close(sock_recv);
            return 1;
        }
        if ((newsd = accept(sock_recv, (struct sockaddr *) &addr_opposite, &addrlen)) == -1)
            perror("accept");

    }

    else {

        if ((newsd = accept(sock_recv, (struct sockaddr *) &addr_opposite, &addrlen)) == -1)
            perror("accept");
        addr_opposite.sin_port = htons(SERVERPORT);
        if (connect(sock_send, (struct sockaddr *) &addr_opposite, sizeof(addr_opposite)) == -1) {
            perror("connect");
            close(sock_send);
            close(sock_recv);
            return 1;
        }
    }
    //////////////////////////////////////////////////////////////////
    ////////////////////////////////两个锁用来同步不同线程///////////////
    pthread_mutex_init(&g_lock_send, NULL);
    pthread_cond_init(&g_cond_send, NULL);
    pthread_mutex_init(&g_lock_recv, NULL);
    pthread_cond_init(&g_cond_recv, NULL);
    //////////////////////////////////////////////////////////////////
    fd = open("/dev/video0", O_RDWR, 0);
    if (fd == -1) {
        perror("open");
        close(fd);
        return 0;
    }


    mark();
    localMem();
    ret = pthread_create(&tid1, NULL, pthread_video, NULL);
    pthread_t tGui;
    pthread_create(&tGui, NULL, guiMain, NULL);
    ret = pthread_create(&tid3, NULL, pthread_rev, &newsd);
    ret = pthread_create(&tid2, NULL, pthread_snd, &sock_send);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    close(sock_recv);
    close(sock_send);
    close(newsd);
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
