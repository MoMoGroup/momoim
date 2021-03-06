#include <math.h>
#include <logger.h>
#include "common.h"

//绘制好友列表的函数
//const UserInfo *userInfo   要重画的好友信息
//int draw_color 			好友头像色彩

GdkPixbuf *DrawFriend(const UserInfo *userInfo, int draw_color)
{
    GdkPixbuf *pixbuf;
    cairo_t *cr;
    cairo_surface_t *surface;


    char filename[256];
    HexadecimalConversion(filename, userInfo->icon);//计算一个文件名
//加载一个图片
    cairo_surface_t *new_friend_surface;
    new_friend_surface = cairo_image_surface_create_from_png(filename);
    int w = cairo_image_surface_get_width(new_friend_surface);//计算图片大小
    int h = cairo_image_surface_get_height(new_friend_surface);
//创建画布
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 260, 60);
//创建画笔
    cr = cairo_create(surface);
    cairo_save(cr);
    cairo_arc(cr, 30, 30, 30, 0, M_PI * 2);
    cairo_clip(cr);
//缩放    pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 60);

    cairo_scale(cr, 60.0 / w, 60.0 / h);
//把画笔和图片相结合。
    cairo_set_source_surface(cr, new_friend_surface, 0, 0);

////把图用画笔画在画布中
    if (draw_color)
    {
        cairo_paint(cr);//1代表在线
    }
    else
    {
        cairo_paint_with_alpha(cr, 0.4);//0不在线
    }


    cairo_restore(cr);
//设置源的颜色
    cairo_set_source_rgb(cr, 0, 0, 0);
//从图像的w+10,30区域开始加入字体
    cairo_move_to(cr, 90, 40);
    cairo_set_font_size(cr, 20);
    cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_show_text(cr, userInfo->nickName);
    log_info("更改资料", "昵称%s\n", userInfo->nickName);

    cairo_destroy(cr);
    cairo_surface_destroy(new_friend_surface);

    pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 60);
    cairo_surface_destroy(surface);
    return pixbuf;
}