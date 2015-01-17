#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <logger.h>
#include <pwd.h>
#include <sys/stat.h>
#include <imcommon/friends.h>
#include <arpa/inet.h>
#include <math.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "newuser.h"
#include "PopupWinds.h"
#include "common.h"
#include "chart.h"

static GtkWidget *imageremember, *imagehead,*ssun, *imagelandbut, *imageregistered, *imageclosebut, *imagecancel;
GtkWidget *LoginWindowUserNameBox, *LoginWindowPassWordBox;
const gchar *name, *pwd;
static pthread_t threadMainLoop;
static GtkWidget *window;
typedef struct cunchu
{
    char cunchu_name[40];
    char cunchu_pwd[16];
    uint32_t cunchu_uid;
    char cunchu_lujing[16];
};
struct cunchu str_cunchu[20];
FILE *passwdfp;
int flag_cunchu = 0;
int FlagRemember = 0;
int FirstPwd = 0;
char mulu_username[80] = "", mulu_benji[80] = "";
GtkStyleContext *combostyle;
GtkStyleProvider *comboprovider;
const gchar *combostring;

static cairo_surface_t *sbackground, *sheadimage, *swhite, *slandbut1, *slandbut2, *slandbut3, *saccount, *spasswd, *sremember1, *sremember2;
static cairo_surface_t *sregistered1, *sregistered2, *sclosebut1, *sclosebut2, *sclosebut3, *scancel10_1, *scancel10_2, *scancel10_3;
static GtkWidget *loginLayout, *pendingLayout, *frameLayout;
static GtkEventBox *remember_box, *sunevent_box, *landbutevent_box, *registeredevent_box, *closebutevent_box, *cancelevent_box, *backgroundevent_box, *waitevent_box;

//IP填写界面所需变量
static GtkWidget *IpFillinWind, *IpLayout;
static GtkWidget *IpBackg, *IpSure, *IpFillText;
static cairo_surface_t *IpFillBackgface, *IpAnniuface, *IpAnniuface1;
char checkmulu_ip[80];

void open_setting_file(FILE *fp)
{
    UserWordInfo.coding_font_color = (gchar *) malloc(500);
    UserWordInfo.codinglen = fread(UserWordInfo.coding_font_color, 1, 500, fp);
    UserWordInfo.description = pango_font_description_new();
    gchar *ptext = UserWordInfo.coding_font_color, *ptext_end = UserWordInfo.coding_font_color + UserWordInfo.codinglen;
    while (ptext < ptext_end)
    {
        if (*ptext == '\0')
        {
            switch (*(ptext + 1))
            {
                case  1:      //字体类型
                {
                    int i;

                    for (i = 2; ptext[i]; ++i)
                    {

                    }
                    UserWordInfo.font = (gchar *) malloc(50);
                    pango_font_description_set_family(UserWordInfo.description, ptext + 2);
                    memcpy(UserWordInfo.font, ptext + 2, i - 2);
                    ptext = ptext + i + 1;
                    break;

                };

                case 2: //是否斜体
                {
                    ptext = ptext + 2;
                    int style_value = *ptext;
                    if (style_value == 1)
                    {
                        pango_font_description_set_style(UserWordInfo.description, PANGO_STYLE_ITALIC);
                        UserWordInfo.style = PANGO_STYLE_ITALIC;
                    }
                    else
                    {
                        UserWordInfo.style = PANGO_STYLE_NORMAL;
                    }
                    ptext++;
                    break;
                };
                case 3:   //宽度
                {
                    ptext = ptext + 2;
                    int weight_value = 0;
                    memcpy(&weight_value, ptext, 2);
                    pango_font_description_set_weight(UserWordInfo.description, weight_value);
                    UserWordInfo.weight = weight_value;
                    ptext = ptext + 2;
                    break;
                };
                case 4:
                {
                    ptext = ptext + 2;
                    gint size_value;
                    size_value = *ptext;
                    pango_font_description_set_size(UserWordInfo.description, size_value * PANGO_SCALE);
                    UserWordInfo.size = size_value;
                    ptext++;
                    break;
                };
                case 5:
                {
                    ptext = ptext + 2;

                    memcpy(&UserWordInfo.color_red, ptext, 2);
                    memcpy(&UserWordInfo.color_green, ptext + 2, 2);
                    memcpy(&UserWordInfo.color_blue, ptext + 4, 2);
                    g_print("the red is %u\n", UserWordInfo.color_red);
                    g_print("the green is %u\n", UserWordInfo.color_green);
                    g_print("the blue is %u\n", UserWordInfo.color_blue);

                    ptext = ptext + 6;
                    break;
                }
                default:
                {
                    pango_font_description_free(UserWordInfo.description);
                    break;
                }
            }
        }
    }
}

gboolean MyThread(gpointer user_data)//合并
{

    //这里是高铭的代码。用来初始化音视频的数据
    //the_log_request_friend_discover.uid=-1;
    //the_log_request_friend_discover.requset_reason=-1;
    //初始化音视频结束
    gtk_widget_destroy(window);
    FILE *fp;
    char wordfile[256];
    sprintf(wordfile, "%s/.momo/%u/setting", getpwuid(getuid())->pw_dir, CurrentUserInfo->uid);
    if ((fp = fopen(wordfile, "r")) != NULL)
    {
        open_setting_file(fp);

    }

    else
    {
        UserWordInfo.font = (gchar *) malloc(50);
        UserWordInfo.description = pango_font_description_new();
        pango_font_description_set_family(UserWordInfo.description, "Sans");
        pango_font_description_set_style(UserWordInfo.description, PANGO_STYLE_NORMAL);
        pango_font_description_set_weight(UserWordInfo.description, PANGO_WEIGHT_NORMAL);
        pango_font_description_set_size(UserWordInfo.description, 14 * PANGO_SCALE);
        memcpy(UserWordInfo.font, "Sans", strlen("Sans"));
        UserWordInfo.style = PANGO_STYLE_NORMAL;
        UserWordInfo.weight = PANGO_WEIGHT_NORMAL;
        UserWordInfo.size = 14;
        UserWordInfo.color_red = 0;
        UserWordInfo.color_green = 0;
        UserWordInfo.color_blue = 0;

    }
    MainInterFace();
    return 0;
}//合并

gboolean DestroyLayout(gpointer user_data)
{
    popup("莫默告诉你：", user_data);
    gtk_widget_hide(pendingLayout);
    gtk_widget_show_all(loginLayout);
    free(user_data);
    return FALSE;
}

//窗口
static void
create_surfaces1()
{
    ssun = ChangeThem_file("1.gif");
    sbackground = ChangeThem_png("背景2.png");
    sheadimage = ChangeThem_png("头像.png");
    swhite = ChangeThem_png("白色.png");
    slandbut1 = ChangeThem_png("登陆按钮.png");
    slandbut2 = ChangeThem_png("登陆按钮2.png");
    slandbut3 = ChangeThem_png("登陆按钮3.png");
    saccount = ChangeThem_png("账号.png");
    spasswd = ChangeThem_png("密码.png");
    sregistered1 = ChangeThem_png("注册账号.png");
    sregistered2 = ChangeThem_png("注册账号2.png");
    sclosebut1 = ChangeThem_png("关闭按钮1.png");
    sclosebut2 = ChangeThem_png("关闭按钮2.png");
    sclosebut3 = ChangeThem_png("关闭按钮3.png");
    scancel10_1 = ChangeThem_png("取消1.png");
    scancel10_2 = ChangeThem_png("取消2.png");
    scancel10_3 = ChangeThem_png("取消3.png");
    sremember1 = ChangeThem_png("记住密码1.png");
    sremember2 = ChangeThem_png("记住密码2.png");
}

static void
destroy_surfaces()
{
    g_print("destroying surfaces1");
    cairo_surface_destroy(sbackground);
    cairo_surface_destroy(sheadimage);
    cairo_surface_destroy(swhite);
    cairo_surface_destroy(slandbut1);
    cairo_surface_destroy(slandbut2);
    cairo_surface_destroy(slandbut3);
    cairo_surface_destroy(saccount);
    cairo_surface_destroy(spasswd);
    cairo_surface_destroy(sregistered1);
    cairo_surface_destroy(sregistered2);
    cairo_surface_destroy(sclosebut1);
    cairo_surface_destroy(sclosebut2);
    cairo_surface_destroy(sclosebut3);
    cairo_surface_destroy(scancel10_1);
    cairo_surface_destroy(scancel10_2);
    cairo_surface_destroy(scancel10_3);
    cairo_surface_destroy(sremember1);
    cairo_surface_destroy(sremember2);
}

extern int DeleteEvent()
{
    gtk_main_quit();
    return TRUE;
}

void *sendhello(void *M)
{
    mysockfd();
    return 0;
}

/*获取登陆信息，显示登陆框*/
void on_button_clicked()
{
//获取登录名和密码
    if (FirstPwd == 0)
    {
        name = gtk_combo_box_text_get_active_text(LoginWindowUserNameBox);
        pwd = gtk_entry_get_text(GTK_ENTRY(LoginWindowPassWordBox));
    }
    else
    {
        char hash[16];
        name = gtk_combo_box_text_get_active_text(LoginWindowUserNameBox);
        pwd = gtk_entry_get_text(GTK_ENTRY(LoginWindowPassWordBox));
        MD5((unsigned char *) pwd, strlen(pwd), hash);//加密存储
        strncpy(pwd, hash, 16);
    }


//判断输入
    if ((strlen(name) != 0) && (strlen(pwd) != 0))
    {
        int charnum, number = 0;
        for (charnum = 0; name[charnum];)
        {
            if ((isalnum(name[charnum]) != 0) || (name[charnum] == '@')
                                                 || (name[charnum] == '.') || (name[charnum] == '-') || (name[charnum] == '_'))
            {
                if (isdigit(name[charnum]) != 0)
                {
                    number++;
                }
                charnum++;
            }
            else
            {
                break;
            }
        }
        if (charnum == strlen(name))
        {
        }
        else
        {
            popup("莫默告诉你：", "包含不合格字符");
            return;
        }
    }
    else
    {
        popup("莫默告诉你：", "请填写登录信息");
        return;
    }
    gtk_widget_hide(loginLayout);//隐藏loginlayout
    //gtk_widget_destroy(layout);销毁layout对话框


    gtk_container_add(GTK_CONTAINER (frameLayout), pendingLayout);

    gtk_widget_show_all(pendingLayout);//显示layout2

    pthread_create(&threadMainLoop, NULL, sendhello, NULL);
}

/*账号更改触发*/
static gint combo_change_event()
{
    int i;
    char buflujing[256];
    FlagRemember = 0;
    gtk_image_set_from_surface((GtkImage *) imageremember, sremember1);//置换不记住图片
    gtk_test_text_set(LoginWindowPassWordBox, "");

    cairo_surface_t *surface, *surfacehead2;//设置默认头像
    static cairo_t *cr;
    //加载一个图片
    surface = ChangeThem_png("头像.png");
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);
    //创建画布
    surfacehead2 = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 164, 164);
    //创建画笔
    cr = cairo_create(surfacehead2);
    //缩放
    cairo_arc(cr, 82, 82, 82, 0, M_PI * 2);
    cairo_clip(cr);
    cairo_scale(cr, 164.0 / w, 164.0 / h);
    //把画笔和图片相结合。
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    gtk_image_set_from_surface((GtkImage *) imagehead, surfacehead2);

    name = gtk_combo_box_text_get_active_text(LoginWindowUserNameBox);

    if (strcmp(name, "") != 0)
    {
        for (i = 0; i < flag_cunchu; ++i) //若账号名本地有则相应取出密码
        {
            if (strcmp(name, str_cunchu[i].cunchu_name) == 0)
            {
                gtk_test_text_set(LoginWindowPassWordBox, str_cunchu[i].cunchu_pwd);
                HexadecimalConversion(buflujing, str_cunchu[i].cunchu_lujing);
                //加载一个图片
                surface = cairo_image_surface_create_from_png(buflujing);
                int w = cairo_image_surface_get_width(surface);
                int h = cairo_image_surface_get_height(surface);
                //创建画布
                surfacehead2 = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 164, 164);
                //创建画笔
                cr = cairo_create(surfacehead2);
                //缩放
                cairo_arc(cr, 82, 82, 82, 0, M_PI * 2);
                cairo_clip(cr);
                cairo_scale(cr, 164.0 / w, 164.0 / h);
                //把画笔和图片相结合。
                cairo_set_source_surface(cr, surface, 0, 0);
                cairo_paint(cr);
                gtk_image_set_from_surface((GtkImage *) imagehead, surfacehead2);

                gtk_image_set_from_surface((GtkImage *) imageremember, sremember2);//显示记住密码
                FlagRemember = 1;
            }
        }
    }
    else
    {
        gtk_test_text_set(LoginWindowPassWordBox, "");
        FlagRemember = 0;
        gtk_image_set_from_surface((GtkImage *) imageremember, sremember1);
    }

    return 0;
}

//密码修改触发
static gint passwd_change_event()
{
    if (FlagRemember == 1)//保存过的密码修改
    {
        int i;
        FlagRemember = 0;
        gtk_image_set_from_surface((GtkImage *) imageremember, sremember1);

        //删除原有账号信息
        for (i = 0; i < flag_cunchu; ++i)
        {
            if (strcmp(name, str_cunchu[i].cunchu_name) == 0)//在数组中查找
            {
                int fd = open(mulu_username, O_RDWR);
                if (fd == -1)
                {
                    log_error("User", "Cannot read user friends file %s.\n", mulu_username);
                    break;
                }
                lseek(fd, 0, SEEK_SET);
                struct stat statBuf;
                if (fstat(fd, &statBuf))
                {
                    break;
                }
                size_t len = (size_t) statBuf.st_size, cpLen;
                char *addr = (char *) malloc(len);
                read(fd, addr, len);
                char *p = addr + 76 * i, *pLine = addr + 76 * (i + 1);
                while (pLine < addr + len)
                {
                    *p++ = *pLine++;
                }
                lseek(fd, 0, SEEK_SET);
                write(fd, addr, p - addr);
                ftruncate(fd, p - addr);
                close(fd);
                free(addr);

                //重新加载下拉框
                memcpy(str_cunchu, "", sizeof(str_cunchu));
                gtk_combo_box_text_remove_all(LoginWindowUserNameBox);//清空原有下拉框内容
                if ((passwdfp = fopen(mulu_username, "r")) != NULL)
                {
                    int i = 0;
                    while ((fread(str_cunchu + i, 1, 76, passwdfp) != NULL) && (i < 20))
                    {
                        gtk_combo_box_text_append(LoginWindowUserNameBox, NULL, str_cunchu[i].cunchu_name);
                        ++i;
                    }
                    flag_cunchu = i;
                    fclose(passwdfp);
                }
            }
        }
    }
}

//背景
static gint background_button_press_event(GtkWidget *widget,
                                          GdkEventButton *event, gpointer data)
{
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;

}

//等待的背景、第二页面
static gint wait_button_press_event(GtkWidget *widget,   //第二界面的窗体移动
                                    GdkEventButton *event, gpointer data)
{
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;
}

//登陆按钮
static gint landbut_button_press_event(GtkWidget *widget,
                                       GdkEventButton *event, gpointer data)
{

    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut2);
    }

    return 0;
}

// 鼠标抬起事件
static gint landbut_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                         gpointer data)
{
    if (event->button == 1)  //判断是否在登陆区域中，设置登陆按钮
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut1);
        on_button_clicked();
    }
    return 0;
}

static gint landbut_enter_notify_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)         // 鼠标移动事件
{

    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut3);
    return 0;
}

static gint landbut_leave_notify_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut1);
    return 0;
}

static gint registered_button_press_event(GtkWidget *widget,
                                          GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        // 设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) imageregistered, sregistered2);

    }
    return 0;

}

// 鼠标抬起事件
static gint registered_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                            gpointer data)
{
    newface(); //调用注册界面
    return 0;
}

static gint registered_enter_notify_event(GtkWidget *widget, GdkEventButton *event,
                                          gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    return 0;
}

static gint registered_leave_notify_event(GtkWidget *widget, GdkEventButton *event,
                                          gpointer data)         // 离开事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    return 0;
}

static gint closebut_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut2); //置换图标
    }
    return 0;
}

// 鼠标抬起事件
static gint closebut_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut1);  //设置关闭按钮

        //gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));

        DeleteEvent();
    }
    return 0;
}

static gint closebut_enter_notify_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut3);
    return 0;
}

static gint closebut_leave_notify_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut1);
    return 0;
}


static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {   //设置第二界面取消按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_3);//设置鼠标光标
    }

    return 0;
}

// 鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)
{
    if (event->button == 1)
    {                                         //设置取消按钮
        gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_1);
        pthread_cancel(threadMainLoop);
        CRPClose(sockfd);
        gtk_widget_hide(pendingLayout);
        gtk_widget_show_all(loginLayout);
    }

    return 0;
}

static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_2);
    return 0;
}

static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)         // 鼠标离开事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_1);
    return 0;
}

//记住密码的鼠标按下事件
static gint remember_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        if (FlagRemember == 0)
        {
            name = gtk_combo_box_text_get_active_text(LoginWindowUserNameBox);
            pwd = gtk_entry_get_text(GTK_ENTRY(LoginWindowPassWordBox));
            if ((strcmp(name, "") != 0) && (strcmp(pwd, "") != 0)) //不为空
            {
//                MD5((unsigned char *) pwd, strlen(pwd), hash);//加密存储
//                gtk_test_text_set(passwd, hash);//加密后的密码写入密码框
                FirstPwd = 1;
                gtk_image_set_from_surface((GtkImage *) imageremember, sremember2);//置换记住图标
                FlagRemember = 1;//置换标志
            }
            else
            {
                gtk_image_set_from_surface((GtkImage *) imageremember, sremember1);//置换取消记住图标
                FlagRemember = 0;
                popup("莫默告诉你", "请输入完整账号信息");
            }
        }
        else
        {
            //删除账号
            int i;
            gtk_image_set_from_surface((GtkImage *) imageremember, sremember1);//置换取消记住图标
            FlagRemember = 0;

            //从文件中删除信息
            for (i = 0; i < flag_cunchu; ++i)
            {
                if (strcmp(name, str_cunchu[i].cunchu_name) == 0)//在数组中查找
                {
                    int fd = open(mulu_username, O_RDWR);
                    if (fd == -1)
                    {
                        log_error("User", "Cannot read user friends file %s.\n", mulu_username);
                        break;
                    }
                    lseek(fd, 0, SEEK_SET);
                    struct stat statBuf;
                    if (fstat(fd, &statBuf))
                    {
                        break;
                    }
                    size_t len = (size_t) statBuf.st_size, cpLen;
                    char *addr = (char *) malloc(len);
                    read(fd, addr, len);
                    char *p = addr + 76 * i, *pLine = addr + 76 * (i + 1);
                    while (pLine < addr + len)
                    {
                        *p++ = *pLine++;
                    }
                    lseek(fd, 0, SEEK_SET);
                    write(fd, addr, p - addr);
                    ftruncate(fd, p - addr);
                    close(fd);
                    free(addr);

                    //重新加载下拉框
                    memcpy(str_cunchu, "", sizeof(str_cunchu));
                    gtk_combo_box_text_remove_all(LoginWindowUserNameBox);//清空原有下拉框内容
                    if ((passwdfp = fopen(mulu_username, "r")) != NULL)
                    {
                        int i = 0;
                        while ((fread(str_cunchu + i, 1, 76, passwdfp) != NULL) && (i < 20))
                        {
                            gtk_combo_box_text_append(LoginWindowUserNameBox, NULL, str_cunchu[i].cunchu_name);
                            ++i;
                        }
                        flag_cunchu = i;
                        fclose(passwdfp);
                    }
                }
            }
            gtk_test_text_set(LoginWindowPassWordBox, "");
        }
    }

    return 0;
}


//填写IP背景的eventbox拖曳窗口
static gint ipbackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(IpFillinWind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;
}

//保存
//鼠标点击事件
static gint ipsure_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(IpFillinWind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//保存
//鼠标抬起事件
static gint ipsure_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        const gchar *ipstrings;
        struct in_addr inp;
        ipstrings = gtk_entry_get_text(GTK_ENTRY(IpFillText));
        if ((inet_aton(ipstrings, &inp)) == 1)
        {
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in server_addr = {
                    .sin_family=AF_INET,
                    .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
                    //.sin_addr.s_addr=inp.s_addr,
                    .sin_port=htons(8014)
            };
            if (connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
            {
                perror("Connect");
                popup("莫默告诉你：", "连接不到服务器");
                return 0;
            }
            CRPContext sockfd = CRPOpen(fd);
            log_info("Hello", "Sending Hello\n");
            CRPHelloSend(sockfd, 0, 1, 1, 1, 0);
            CRPBaseHeader *header;
            log_info("Hello", "Waiting OK\n");
            header = CRPRecv(sockfd);
            if (header == NULL || header->packetID != CRP_PACKET_OK)
            {
                log_error("Hello", "Recv Packet:%d\n", header->packetID);
                popup("莫默告诉你：", "连接不到服务器");
                return 1;
            }
            else
            {
                FILE *ipfp;
                ipfp = fopen(data, "a+");
                fwrite(ipstrings, 1, strlen(ipstrings), ipfp);
                fclose(ipfp);
                gtk_widget_destroy(IpFillinWind);
                loadloginLayout(NULL);//加载登陆界面
            }

        }
        else
        {
            popup("莫默告诉你：", "请输入正确的IP地址");
        }
    }
    return 0;
}

//保存
//鼠标移动事件
static gint ipsure_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(IpFillinWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) IpSure, IpAnniuface1); //置换图标
    return 0;
}

//保存
//鼠标离开事件
static gint ipsure_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(IpFillinWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) IpSure, IpAnniuface);
    return 0;
}

int FillinIp(char *filename)
{
    static GtkEventBox *IpBackg_event_box, *IpSure_event_box;
    IpFillinWind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(IpFillinWind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW(IpFillinWind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(IpFillinWind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(IpFillinWind), 432, 238);

    IpLayout = gtk_fixed_new();
    IpFillBackgface = cairo_image_surface_create_from_png("首次登陆.png");
    IpAnniuface = ChangeThem_png("提示框按钮1.png");
    IpAnniuface1 = ChangeThem_png("提示框按钮2.png");

    gtk_container_add(GTK_CONTAINER(IpFillinWind), IpLayout);

    IpBackg = gtk_image_new_from_surface(IpFillBackgface);
    IpSure = gtk_image_new_from_surface(IpAnniuface);

    IpBackg_event_box = BuildEventBox(IpBackg,
                                      G_CALLBACK(ipbackg_button_press_event),
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL);
    gtk_fixed_put(GTK_FIXED(IpLayout), IpBackg_event_box, 0, 0);

    IpSure_event_box = BuildEventBox(IpSure,
                                     G_CALLBACK(ipsure_button_press_event),
                                     G_CALLBACK(ipsure_enter_notify_event),
                                     G_CALLBACK(ipsure_leave_notify_event),
                                     G_CALLBACK(ipsure_button_release_event),
                                     NULL,
                                     filename);
    gtk_fixed_put(GTK_FIXED(IpLayout), IpSure_event_box, 143, 180);

    IpFillText = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(IpLayout), IpFillText, 125, 130);
    gtk_widget_show_all(IpFillinWind);
    return 0;
}

int main(int argc, char *argv[])
{
    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    char checkmulu[80], minglingcp[256], checkmulu_theme[80];
    sprintf(checkmulu, "%s/.momo", getpwuid(getuid())->pw_dir);
    sprintf(checkmulu_ip, "%s/ip", checkmulu);
    sprintf(minglingcp, "cp -r /opt/momo/theme %s/theme/", checkmulu);
    mkdir(checkmulu, 0700);
    if (access(checkmulu_ip, 0) != 0)
    {
        system(minglingcp);
        sprintf(checkmulu_theme, "%s/theme/cartoon", checkmulu);
        sprintf(checkmulu, "%s/current_theme", checkmulu);
        symlink(checkmulu_theme, checkmulu);
        FillinIp(checkmulu_ip);
    }
    else
    {
        loadloginLayout("ad");//加载登陆界面
    }
    gtk_main();
    destroy_surfaces();
    return 0;
}

gboolean destoryall(gpointer user_data)
{
    DestoryMainInterface();//销毁主窗口,--maininterface

    FriendInfo *head = FriendInfoHead;
    FriendInfo *p;
    while (head->next)
    {
        p = head->next;
        head->next = p->next;
        if (p->chartwindow)
        {
            gtk_widget_destroy(p->chartwindow);
        }
        free(p);
    }

    loadloginLayout(NULL);
    popup("异地登录", "您的帐号在别处登录，\n 如非本人操作，\n请尽快修改密码");
    return FALSE;
}


gboolean loadloginLayout(gpointer user_data)
{
    //加载loginlayout
    create_surfaces1();
    GtkWidget *imagebackground, *imagewhite, *imageaccount, *imagepasswd;
    GtkWidget *iwait;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    g_signal_connect(G_OBJECT(window), "delete_event",
                     G_CALLBACK(gtk_main_quit), NULL);

    //gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(window), 283, 411);

    iwait = ChangeThem_file("等待.gif");
    imagebackground = gtk_image_new_from_surface(sbackground);
    imagehead = gtk_image_new_from_surface(sheadimage);
    imagewhite = gtk_image_new_from_surface(swhite);
    imagelandbut = gtk_image_new_from_surface(slandbut1);
    imageaccount = gtk_image_new_from_surface(saccount);
    imagepasswd = gtk_image_new_from_surface(spasswd);
    imageregistered = gtk_image_new_from_surface(sregistered1);
    imageclosebut = gtk_image_new_from_surface(sclosebut1);
    imagecancel = gtk_image_new_from_surface(scancel10_1);
    imageremember = gtk_image_new_from_surface(sremember1);

    backgroundevent_box = BuildEventBox(
            imagebackground,
            G_CALLBACK(background_button_press_event),
            NULL, NULL, NULL, NULL, NULL);

    waitevent_box = BuildEventBox(
            iwait,
            G_CALLBACK(wait_button_press_event),
            NULL, NULL, NULL, NULL, NULL);

    landbutevent_box = BuildEventBox(
            imagelandbut,
            G_CALLBACK(landbut_button_press_event),
            G_CALLBACK(landbut_enter_notify_event),
            G_CALLBACK(landbut_leave_notify_event),
            G_CALLBACK(landbut_button_release_event),
            NULL, NULL
                                    );

    sunevent_box = BuildEventBox(
            ssun,
            G_CALLBACK(background_button_press_event),
            NULL, NULL, NULL, NULL, NULL);

    remember_box = BuildEventBox(
            imageremember,
            G_CALLBACK(remember_button_press_event),
            NULL, NULL, NULL, NULL, NULL);

    registeredevent_box = BuildEventBox(
            imageregistered,
            G_CALLBACK(registered_button_press_event),
            G_CALLBACK(registered_enter_notify_event),
            G_CALLBACK(registered_leave_notify_event),
            G_CALLBACK(registered_button_release_event),
            NULL, NULL);

    closebutevent_box = BuildEventBox(
            imageclosebut,
            G_CALLBACK(closebut_button_press_event),
            G_CALLBACK(closebut_enter_notify_event),
            G_CALLBACK(closebut_leave_notify_event),
            G_CALLBACK(closebut_button_release_event),
            NULL, NULL);

    cancelevent_box = BuildEventBox(
            imagecancel,
            G_CALLBACK(cancel_button_press_event),
            G_CALLBACK(cancel_enter_notify_event),
            G_CALLBACK(cancel_leave_notify_event),
            G_CALLBACK(cancel_button_release_event),
            NULL, NULL);

    frameLayout = gtk_layout_new(NULL, NULL);
    pendingLayout = gtk_fixed_new();
    loginLayout = gtk_fixed_new();

    //设置两个输入框
    LoginWindowUserNameBox = gtk_combo_box_text_new_with_entry();
    GtkEntry *nameEntry = GTK_ENTRY(gtk_bin_get_child(LoginWindowUserNameBox));
    gtk_entry_set_width_chars(nameEntry, 19);

    LoginWindowPassWordBox = gtk_entry_new();
    gtk_entry_set_max_length(LoginWindowPassWordBox, 20);//最大输入长度
    //   gtk_combo_box_set_active(LoginWindowUserNameBox, 0); //设置id0为默认的输入
    //   gtk_combo_box_set_active(LoginWindowPassWordBox, 1); //设置id0为默认的输入

    gtk_entry_set_visibility(GTK_ENTRY(LoginWindowPassWordBox), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(LoginWindowPassWordBox), '*');//设置密码不可见

    g_signal_connect(LoginWindowUserNameBox, "changed", G_CALLBACK(combo_change_event), NULL);//账号修改触发
    g_signal_connect(LoginWindowPassWordBox, "changed", G_CALLBACK(passwd_change_event), NULL);//密码修改触发

//从本地读取账号记录
    sprintf(mulu_benji, "%s/.momo", getpwuid(getuid())->pw_dir);//获取本机主目录
    mkdir(mulu_benji, 0700);
    sprintf(mulu_username, "%s/username", mulu_benji);
    // 读取并用结构体数组存储
    if ((passwdfp = fopen(mulu_username, "r")) != NULL)
    {
        int i = 0;
        while ((fread(str_cunchu + i, 1, 76, passwdfp) != NULL) && (i < 20))
        {
            gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(LoginWindowUserNameBox), NULL, str_cunchu[i].cunchu_name);

            ++i;
        }
        flag_cunchu = i;
        fclose(passwdfp);
    }
    gtk_combo_box_set_active(LoginWindowUserNameBox, 0); //设置id0为默认的输入

//放置组件的相对位置
    gtk_fixed_put(GTK_FIXED(loginLayout), GTK_WIDGET(backgroundevent_box), 0, 0);//起始坐标
    gtk_fixed_put(GTK_FIXED(loginLayout), imagehead, 61, 30);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagewhite, 25, 200);
    gtk_fixed_put(GTK_FIXED(loginLayout), GTK_WIDGET(landbutevent_box), 75, 300);
    gtk_fixed_put(GTK_FIXED(loginLayout), imageaccount, 33, 220);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagepasswd, 33, 260);
    gtk_fixed_put(GTK_FIXED(loginLayout), GTK_WIDGET(registeredevent_box), 5, 380);
    gtk_fixed_put(GTK_FIXED(loginLayout), GTK_WIDGET(closebutevent_box), 247, 0);
    gtk_fixed_put(GTK_FIXED(loginLayout), GTK_WIDGET(sunevent_box), 3, 3);
    gtk_fixed_put(GTK_FIXED(loginLayout), GTK_WIDGET(remember_box), 190, 185);
    gtk_fixed_put(GTK_FIXED(pendingLayout), GTK_WIDGET(waitevent_box), 0, 0);
    gtk_fixed_put(GTK_FIXED(pendingLayout), GTK_WIDGET(cancelevent_box), 75, 350);
    gtk_fixed_put(GTK_FIXED(loginLayout), LoginWindowUserNameBox, 82, 215);
    gtk_fixed_put(GTK_FIXED(loginLayout), LoginWindowPassWordBox, 82, 256);


    gtk_container_add(GTK_CONTAINER (window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER (frameLayout), loginLayout);

    gtk_widget_show(landbutevent_box);
    gtk_widget_show_all(window);
    return FALSE;
}