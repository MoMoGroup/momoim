#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <pwd.h>
#include <string.h>
#include <logger.h>
#include <stdlib.h>
#include <bits/string2.h>
#include "ClientSockfd.h"
#include "common.h"
#include "MainInterface.h"
#include "PopupWinds.h"

static cairo_surface_t *Surfaceback, *Surfacesave, *Surfacesave1, *Surfacecancel, *Surfacecancel1, *Surfaceend, *Surfaceend1, *Surfaceend2;
static cairo_surface_t *surfacehead;
int RiliFlag = 0;
guint year = 2011;
guint month = 11;
guint day = 11;

static GtkWidget *Infowind;
static GtkWidget *Infolayout;
static GtkWidget *Infobackground, *Infosave, *Infocancel, *Infoguanbi;
static GtkWidget *iid, *ilevel, *isex, *inickname, *iname, *ibirthday, *iconstellation, *iprovinces, *icity;
static GtkWidget *itel, *ischool, *ipostcode, *ihometown, *icalendar;
static GtkWidget *headicon;


static const char *constellations[] = {
        "水瓶座", "双鱼座", "白羊座", "金牛座",
        "双子座", "巨蟹座", "狮子座", "处女座",
        "天秤座", "天蝎座", "射手座", "摩羯座"
};

static const char *provinces[] = {
        "北京", "上海", "天津", "重庆", "河北", "山西", "辽宁", "吉林",
        "河南", "江苏", "浙江", "安徽", "福建", "江西", "山东",
        "湖北", "湖南", "广东", "海南", "四川", "贵州", "云南",
        "陕西", "甘肃", "青海", "黑龙江",
        "内蒙古", "广西", "西藏", "宁夏", "新疆", "台湾省", "香港", "澳门", "其它"
};
static const char *allcity[35][41] = {
//北京市辖区名称
        {"东城区", "西城区", "崇文区", "宣武区", "朝阳区", "海淀区",
                "丰台区", "石景山区", "房山区", "通州区", "顺义区", "门头沟区",
                "昌平区", "大兴区", "怀柔区", "平谷区", "密云县", "延庆县"},
//上海市辖区名称
        {"黄浦区", "卢湾区", "徐汇区", "长宁区", "静安区", "普陀区",
                "闸北区", "虹口区", "杨浦区", "宝山区", "闵行区", "嘉定区",
                "浦东新区", "金山区", "松江区", "青浦区", "南汇区", "奉贤区", "崇明县"},
//天津市辖区名称
        {"和平区", "河东区", "河西区", "南开区", "河北区", "红桥区",
                "塘沽区", "汉沽区", "大港区", "东丽区", "西青区", "津南区",
                "北辰区", "武清区", "宝坻区", "宁河县", "静海县", "蓟县"},
//重庆市辖区名称
        {"渝中区", "大渡口区", "江北区", "沙坪坝区", "九龙坡区", "南岸区",
                "北碚区", "万盛区", "双桥区", "渝北区", "巴南区", "万县区",
                "涪陵区", "永川市", "合川市", "江津市", "南川市", "长寿县",
                "綦江县", "潼南县", "荣昌县", "壁山县", "大足县", "铜梁县",
                "梁平县", "城口县", "垫江县", "武隆县", "丰都县", "忠 县", "开 县",
                "云阳县", "青龙镇青龙嘴", "奉节县", "巫山县", "巫溪县", "南宾镇", "中和镇", "钟多镇", "联合镇", "汉葭镇"},
//河北省主要城市名称
        {"石家庄", "唐山", "秦皇岛", "邯郸", "邢台", "保定",
                "张家口", "承德", "沧州", "廊坊", "衡水"},
//山西省主要城市名称
        {"太原", "大同", "阳泉", "长治", "晋城", "朔州",
                "晋中", "运城", "忻州", "临汾", "吕梁"},
//辽宁省主要城市名称
        {"沈阳", "大连", "鞍山", "抚顺", "本溪", "丹东",
                "锦州", "营口", "阜新", "辽阳", "盘锦", "铁岭", "朝阳", "葫芦岛"},
//吉林省主要城市名称
        {"长春", "吉林", "四平", "辽源", "通化", "白山",
                "松原", "白城", "延边朝鲜族自治州"},
//河南省主要城市名称
        {"郑州", "开封", "洛阳", "平顶山", "安阳", "鹤壁",
                "新乡", "焦作", "濮阳", "许昌", "漯河", "三门峡",
                "南阳", "商丘", "信阳", "周口", "驻马店", "济源"},
//江苏省主要城市名称
        {"南京", "无锡", "徐州", "常州", "苏州", "南通",
                "连云港", "淮安", "盐城", "扬州", "镇江", "泰州", "宿迁"},
//浙江省主要城市名称
        {"杭州", "宁波", "温州", "嘉兴", "湖州", "绍兴",
                "金华", "衢州", "舟山", "台州", "丽水"},
//安徽省主要城市名称
        {"合肥", "芜湖", "蚌埠", "淮南", "马鞍山", "淮北", "铜陵",
                "安庆", "黄山", "滁州", "阜阳", "宿州", "巢湖", "六安", "亳州", "池州", "宣城"},
//福建省主要城市名称
        {"福州", "厦门", "莆田", "三明", "泉州", "漳州", "南平", "龙岩", "宁德"},
//江西省主要城市名称
        {"南昌", "景德镇", "萍乡", "九江", "新余", "鹰潭", "赣州", "吉安", "宜春", "抚州", "上饶"},
//山东省主要城市名称
        {"济南", "青岛", "淄博", "枣庄", "东营", "烟台", "潍坊",
                "威海", "济宁", "泰安", "日照", "莱芜", "临沂", "德州", "聊城", "滨州", "菏泽"},
//湖北省主要城市名称
        {"武汉", "黄石", "襄樊", "十堰", "荆州", "宜昌", "荆门",
                "鄂州", "孝感", "黄冈", "咸宁", "随州", "恩施", "仙桃", "潜江", "天门", "神农架林区"},
//湖南省主要城市名称
        {"长沙", "株洲", "湘潭", "衡阳", "邵阳", "岳阳", "常德", "张家界",
                "益阳", "郴州", "永州", "怀化市", "娄底", "湘西"},
//广东省主要城市名称
        {"广州", "深圳", "珠海", "汕头", "韶关", "佛山", "江门", "湛江",
                "茂名", "肇庆", "惠州", "梅州", "汕尾", "河源", "阳江", "清远", "东莞", "中山", "潮州", "揭阳", "云浮"},
//海南省主要城市名称
        {"海口", "龙华区", "秀英区", "琼山区", "美兰区", "三亚"},
//四川省主要城市名称
        {"成都", "自贡", "攀枝花", "泸州", "德阳", "绵阳", "广元", "遂宁", "内江", "乐山", "南充",
                "宜宾", "广安", "达州", "眉山", "雅安", "巴中", "资阳", "阿坝州", "甘孜州", "凉山州"},
//贵州省主要城市名称
        {"贵阳", "六盘水", "遵义", "安顺", "铜仁地区", "毕节地区", "黔西南州", "黔东南州", "黔南州"},
//云南省主要城市名称
        {"昆明", "大理", "曲靖", "玉溪", "昭通", "楚雄", "红河", "文山", "思茅", "西双版纳",
                "保山", "德宏", "丽江", "怒江", "迪庆", "临沧"},
//陕西省主要城市名称
        {"西安", "铜川", "宝鸡", "咸阳", "渭南", "延安", "汉中", "榆林", "安康", "商洛"},
//甘肃省主要城市名称
        {"兰州", "嘉峪关", "金昌", "白银", "天水", "武威", "张掖", "平凉",
                "酒泉", "庆阳", "定西", "陇南", "临夏州", "甘南州"},
//青海省主要城市名称
        {"西宁", "海东地区", "海北州", "黄南州", "海南州", "果洛州", "玉树州", "海西州"},
//黑龙江省主要城市名称
        {"哈尔滨", "齐齐哈尔", "鸡西", "鹤岗", "双鸭山", "大庆", "伊春", "佳木斯",
                "七台河", "牡丹江", "黑河", "绥化", "大兴安岭地区"},
//内蒙古自治区主要城市名称
        {"呼和浩特", "包头", "乌海", "赤峰", "通辽", "鄂尔多斯", "呼伦贝尔", "巴彦淖尔",
                "乌兰察布", "兴安盟", "锡林郭勒盟", "阿拉善盟"},
//广西壮族自治区主要城市名称
        {"南宁", "柳州", "桂林", "梧州", "北海", "防城港", "钦州", "贵港", "玉林", "百色",
                "贺州", "河池", "来宾", "崇左"},
//西藏自治区主要城市名称
        {"拉萨", "昌都地区", "山南地区", "日喀则地区", "那曲地区", "阿里地区", "林芝地区"},
//宁夏回族自治区主要城市名称
        {"银川", "石嘴山", "吴市", "固原", "中卫"},
//新疆维吾尔自治区主要城市名称
        {"乌鲁木齐", "克拉玛依", "吐鲁番地区", "哈密地区", "和田地区", "阿克苏地区", "喀什地区",
                "克孜勒苏柯尔克孜自治州", "巴音郭楞蒙古自治州", "昌吉回族自治州", "博尔塔拉蒙古自治州", "伊犁哈萨克自治州", "塔城地区",
                "阿勒泰地区", "石河子", "阿拉尔", "图木舒市", "五家渠"},
//台湾省主要城市名称
        {"台北", "高雄", "基隆", "台中", "台南", "新竹", "嘉义", "台北县", "宜兰县",
                "桃园县", "新竹县", "苗栗县", "台中县", "彰化县", "南投县", "云林县", "嘉义县", "台南县", "高雄县", "屏东县", "澎湖县", "台东县", "花莲县"},
//香港特别行政区主要辖区名称
        {"中西区", "东区", "九龙城区", "观塘区", "南区", "深水埗区", "黄大仙区", "湾仔区",
                "油尖旺区", "离岛区", "葵青区", "北区", "西贡区", "沙田区", "屯门区", "大埔区", "荃湾区", "元朗区"},
//澳门地区
        {"澳门地区"},
//其它地区
        {"其它地区"}};
static const char *xingzuo[12][2] = {
        {"魔蝎座", "水瓶座"}, {"水瓶座", "双鱼座"}, {"双鱼座", "白羊座"},
        {"白羊座", "金牛座"}, {"金牛座", "双子座"}, {"双子座", "巨蟹座"},
        {"巨蟹座", "狮子座"}, {"狮子座", "处女座"}, {"处女座", "天秤座"},
        {"天秤座", "天蝎座"}, {"天蝎座", "射手座"}, {"射手座", "魔蝎座"}
};

int sheng_change_city() {
    const gchar *buf;
    int shengfen = 0;
    buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) iprovinces);
    for (int i = 0; i < 35; ++i) {
        if (strcmp(provinces[i], buf) == 0) {
            shengfen = i;
            break;
        }
    }
    gtk_combo_box_text_remove_all(icity);
    for (int j = 0; j < 41; ++j) {
        gtk_combo_box_text_append(icity, NULL, allcity[shengfen][j]);
    }
    return 0;
}

int calendar_change_birthday() {
    if (RiliFlag == 0) {
        icalendar = gtk_calendar_new();
        char nian[4] = {0}, yue[2] = {0}, ri[2] = {0};
        strncpy(nian, CurrentUserInfo->birthday, 4);
        strncpy(yue, CurrentUserInfo->birthday + 5, 2);
        if (yue[1] == '-')
            yue[1] = 0;
        if (CurrentUserInfo->birthday[6] != '-') {
            ri[0] = CurrentUserInfo->birthday[8];
            ri[1] = CurrentUserInfo->birthday[9];
        } else {
            ri[0] = CurrentUserInfo->birthday[7];
            ri[1] = CurrentUserInfo->birthday[8];
        }
        if (ri[1] == '-')
            ri[1] = 0;
        log_info("年年", "%s%s%s", nian, yue, ri);
        int i = 0;
        i = atoi(yue) - 1;
        gtk_calendar_select_month(GTK_CALENDAR(icalendar), (guint) i, (guint) atoi(nian));
        gtk_calendar_select_day(GTK_CALENDAR(icalendar), (guint) atoi(ri));
        gtk_fixed_put(GTK_FIXED(Infolayout), icalendar, 140, 263);
        gtk_widget_show(icalendar);
        RiliFlag = 1;
    }
    else {
        gtk_widget_hide(icalendar);
        RiliFlag = 0;
    }
    return 0;
}

static void create_infofaces() {

    Surfaceback = cairo_image_surface_create_from_png("资料.png");
    Surfacesave = cairo_image_surface_create_from_png("更新.png");
    Surfacesave1 = cairo_image_surface_create_from_png("更新2.png");
    Surfacecancel = cairo_image_surface_create_from_png("资料取消.png");
    Surfacecancel1 = cairo_image_surface_create_from_png("资料取消2.png");
    Surfaceend = cairo_image_surface_create_from_png("关闭按钮1.png");
    Surfaceend1 = cairo_image_surface_create_from_png("关闭按钮2.png");
    Surfaceend2 = cairo_image_surface_create_from_png("关闭按钮3.png");

    Infobackground = gtk_image_new_from_surface(Surfaceback);
    Infosave = gtk_image_new_from_surface(Surfacesave);
    Infocancel = gtk_image_new_from_surface(Surfacecancel);
    Infoguanbi = gtk_image_new_from_surface(Surfaceend);
    gtk_widget_set_size_request(GTK_WIDGET(Infobackground), 550, 488);
}

static void destroy_infosurfaces() {
    g_print("destroying infoface");
    cairo_surface_destroy(Surfaceback);
    cairo_surface_destroy(Surfacesave);
    cairo_surface_destroy(Surfacesave1);
    cairo_surface_destroy(Surfacecancel);
    cairo_surface_destroy(Surfacecancel1);
    cairo_surface_destroy(Surfaceend);
    cairo_surface_destroy(Surfaceend1);
    cairo_surface_destroy(Surfaceend2);
}

int infoupdate(CRPBaseHeader *header, void *data)//资料更新处理函数
{
    switch (header->packetID) {
        case CRP_PACKET_OK: {
            log_info("请求添加", "收到OKBAO\n");
            return 0;
        };
        case CRP_PACKET_FAILURE: {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            log_info("FAILURe reason", infodata->reason);
            break;
        };
    }
    return 0;
}

int infosockfd() {
    UserInfo weinfo = *CurrentUserInfo;
    const gchar *buf;
    buf = gtk_entry_get_text(GTK_ENTRY(inickname));
    if (strlen(buf) == 0)
        popup("莫默告诉你", "给自己起个昵称");
    memset(weinfo.nickName, 0, strlen(weinfo.nickName));
    memcpy(weinfo.nickName, buf, strlen(buf));

    buf = gtk_entry_get_text(GTK_ENTRY(iname));
    memset(weinfo.name, 0, strlen(weinfo.name));
    memcpy(weinfo.name, buf, strlen(buf));

    buf = gtk_entry_get_text(GTK_ENTRY(itel));
    memset(weinfo.tel, 0, strlen(weinfo.tel));
    memcpy(weinfo.tel, buf, strlen(buf));
    log_info("电话", weinfo.tel);

    buf = gtk_entry_get_text(GTK_ENTRY(ipostcode));
    memset(weinfo.postcode, 0, strlen(weinfo.postcode));
    memcpy(weinfo.postcode, buf, strlen(buf));
    log_info("更新资料", weinfo.postcode);

    buf = gtk_entry_get_text(GTK_ENTRY(ischool));
    memset(weinfo.school, 0, strlen(weinfo.school));
    memcpy(weinfo.school, buf, strlen(buf));
    log_info("更新资料", weinfo.school);

    buf = gtk_entry_get_text(GTK_ENTRY(ihometown));
    memset(weinfo.hometown, 0, strlen(weinfo.hometown));
    memcpy(weinfo.hometown, buf, strlen(buf));
    log_info("更新资料", weinfo.hometown);

    buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) isex);
    if (strcmp(buf, "男") == 0)
        weinfo.sex = 1;
    else
        weinfo.sex = 0;

    buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) iprovinces);
    for (int i = 0; i < 35; ++i) {
        if (strcmp(provinces[i], buf) == 0) {
            memset(weinfo.provinces, 0, strlen(weinfo.provinces));
            memcpy(weinfo.provinces, buf, strlen(buf));
            break;
        }
    }

    buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) icity);
    for (int i = 0; i < 35; ++i)
        for (int j = 0; allcity[i][j]; j++) {
            if (strcmp(allcity[i][j], buf) == 0) {
                memset(weinfo.city, 0, strlen(weinfo.city));
                memcpy(weinfo.city, buf, strlen(buf));
                break;
            }
        }

    gtk_calendar_get_date(GTK_CALENDAR(icalendar), &year, &month, &day);/*取得选择的年月日*/
    memset(weinfo.birthday, 0, strlen(weinfo.birthday));

    int x = month + 1;
    int y = day / 21;
    sprintf(buf, "%d-%d-%d", year, x, day);
    memcpy(weinfo.birthday, buf, strlen(buf));
    //buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) iconstellation);
    buf = xingzuo[month][y];
    for (int i = 0; i < 12; ++i) {
        if (strcmp(constellations[i], buf) == 0) {
            weinfo.constellation = i;
            break;
        }
    }
    //log_info("获取的日期", "Year:%d Month:%d Day:%d DATE:%s", year, month, day, buf);

    session_id_t newinfoid = CountSessionId();
    AddMessageNode(newinfoid, infoupdate, NULL);
    CRPInfoDataSend(sockfd, newinfoid, 0, &weinfo);
    return 0;
}

//背景的eventbox拖曳窗口
static gint Infobackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
        gtk_widget_hide(icalendar);
        RiliFlag = 0;
    }
    return 0;
}

//更新
//鼠标点击事件
static gint save_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//更新
//鼠标抬起事件
static gint save_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        infosockfd();
        const gchar *buf;
        buf = gtk_entry_get_text(GTK_ENTRY(inickname));
        if (strlen(buf) != 0) {
            destroy_infosurfaces();
            gtk_widget_destroy(Infowind);
        }
    }
    return 0;
}

//更新
//鼠标移动事件
static gint save_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave1); //置换图标
    return 0;
}

//更新
//鼠标离开事件
static gint save_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave);
    return 0;
}

//取消
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//取消
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        destroy_infosurfaces();
        gtk_widget_destroy(Infowind);
    }

    return 0;
}

//取消
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infocancel, Surfacecancel1); //置换图标
    return 0;
}

//取消
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infocancel, Surfacecancel);

    return 0;
}

//关闭
//鼠标点击事件
static gint guanxx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend1); //置换图标
    }
    return 0;
}

//关闭
//鼠标抬起事件
static gint guanxx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        destroy_infosurfaces();
        gtk_widget_destroy(Infowind);
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend2);
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend);
    return 0;
}

void infotv() {

    char infohead[80] = {0};
    sprintf(infohead, "%s/.momo/%u/head.png", getpwuid(getuid())->pw_dir, CurrentUserInfo->uid);
    surfacehead = cairo_image_surface_create_from_png(infohead);
    headicon = gtk_image_new_from_surface(surfacehead);
    gtk_fixed_put(GTK_FIXED(Infolayout), headicon, 23, 16);

    char idstring[80] = {0};//id
    sprintf(idstring, "%d", CurrentUserInfo->uid);
    iid = gtk_label_new(idstring);
    gtk_fixed_put(GTK_FIXED(Infolayout), iid, 240, 30);
    memset(idstring, 0, strlen(idstring));
    sprintf(idstring, "等级：%d", CurrentUserInfo->level);
    ilevel = gtk_label_new(idstring);
    gtk_fixed_put(GTK_FIXED(Infolayout), ilevel, 200, 50);

    inickname = gtk_entry_new();//昵称
    gtk_entry_set_max_length(inickname, 8);
    gtk_entry_set_has_frame((GtkEntry *) inickname, FALSE);
    gtk_entry_set_text(inickname, CurrentUserInfo->nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), inickname, 58, 175);

    iname = gtk_entry_new();//姓名
    gtk_entry_set_max_length(iname, 4);
    gtk_entry_set_has_frame((GtkEntry *) iname, FALSE);
    gtk_entry_set_text(iname, CurrentUserInfo->name);
    gtk_fixed_put(GTK_FIXED(Infolayout), iname, 48, 235);


    ipostcode = gtk_entry_new();//邮编
    gtk_entry_set_max_length(ipostcode, 6);
    gtk_entry_set_has_frame((GtkEntry *) ipostcode, FALSE);
    gtk_entry_set_text(ipostcode, CurrentUserInfo->postcode);
    gtk_fixed_put(GTK_FIXED(Infolayout), ipostcode, 305, 345);


    ischool = gtk_entry_new();//毕业院校
    gtk_entry_set_max_length(ischool, 10);
    gtk_entry_set_has_frame((GtkEntry *) ischool, FALSE);
    gtk_entry_set_text(ischool, CurrentUserInfo->school);
    gtk_fixed_put(GTK_FIXED(Infolayout), ischool, 75, 375);

    ihometown = gtk_entry_new();//故乡
    gtk_entry_set_max_length(ihometown, 30);
    gtk_entry_set_has_frame((GtkEntry *) ihometown, FALSE);
    gtk_entry_set_text(ihometown, CurrentUserInfo->hometown);
    gtk_fixed_put(GTK_FIXED(Infolayout), ihometown, 48, 403);

    itel = gtk_entry_new();//电话
    gtk_entry_set_max_length(itel, 11);
    gtk_entry_set_has_frame((GtkEntry *) itel, FALSE);
    log_info("tel", CurrentUserInfo->tel);
    gtk_entry_set_text(itel, CurrentUserInfo->tel);
    gtk_fixed_put(GTK_FIXED(Infolayout), itel, 48, 347);

    isex = gtk_combo_box_text_new();//性别
    gtk_combo_box_text_append(isex, "0", "女");
    gtk_combo_box_text_append(isex, "1", "男");
    if (1 == CurrentUserInfo->sex) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(isex), 1);
    }
    else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(isex), 0);
    }
    gtk_fixed_put(GTK_FIXED(Infolayout), isex, 305, 225);
    gtk_widget_show(isex);

    iconstellation = gtk_combo_box_text_new();//星座
    for (int i = 0; i < 12; ++i) {
        gtk_combo_box_text_append(iconstellation, NULL, constellations[i]);
    }
    for (int i = 0; i < 12; ++i) {
        if (CurrentUserInfo->constellation == i) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(iconstellation), i);
            break;
        }
    }
    gtk_fixed_put(GTK_FIXED(Infolayout), iconstellation, 305, 260);
    gtk_widget_show(iconstellation);

    iprovinces = gtk_combo_box_text_new();//省份
    icity = gtk_combo_box_text_new();//城市
    g_signal_connect(iprovinces, "changed", G_CALLBACK(sheng_change_city), NULL);
    int weizhi = 0;
    for (int i = 0; i < 35; ++i) {
        gtk_combo_box_text_append(iprovinces, NULL, provinces[i]);
    }
    for (int i = 0; i < 35; ++i) {
        if (strcmp(provinces[i], CurrentUserInfo->provinces) == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(iprovinces), i);
            weizhi = i;
            break;
        }
        else {
            gtk_combo_box_set_active(GTK_COMBO_BOX(iprovinces), 0);
        }
    }
    for (int j = 0; allcity[weizhi][j]; ++j) {
        if (strcmp(allcity[weizhi][j], CurrentUserInfo->city) == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(icity), j);
            break;
        }
        else {
            gtk_combo_box_set_active(GTK_COMBO_BOX(icity), 0);
        }
    }
    gtk_fixed_put(GTK_FIXED(Infolayout), iprovinces, 48, 290);
    gtk_fixed_put(GTK_FIXED(Infolayout), icity, 305, 290);
    gtk_widget_show(iprovinces);
    gtk_widget_show(icity);

    ibirthday = gtk_button_new_with_label(CurrentUserInfo->birthday);//生日
    gtk_fixed_put(GTK_FIXED(Infolayout), ibirthday, 48, 263);
    g_signal_connect(ibirthday, "clicked", G_CALLBACK(calendar_change_birthday), NULL);

}

int info() {

    static GtkEventBox *Infobackg_event_box, *Save_event_box, *Cancel_event_box, *Guanxx_event_box;
    Infowind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(Infowind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (Infowind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(Infowind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(Infowind), 550, 488);

    Infolayout = gtk_fixed_new();
    create_infofaces();
    gtk_container_add(GTK_CONTAINER(Infowind), Infolayout);

    Infobackg_event_box = BuildEventBox(Infobackground,
                                        G_CALLBACK(Infobackg_button_press_event),
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Infobackg_event_box, 0, 0);

    Save_event_box = BuildEventBox(Infosave,
                                   G_CALLBACK(save_button_press_event),
                                   G_CALLBACK(save_enter_notify_event),
                                   G_CALLBACK(save_leave_notify_event),
                                   G_CALLBACK(save_button_release_event),
                                   NULL,
                                   NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Save_event_box, 350, 440);

    Cancel_event_box = BuildEventBox(Infocancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     G_CALLBACK(cancel_enter_notify_event),
                                     G_CALLBACK(cancel_leave_notify_event),
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(Infoguanbi,
                                     G_CALLBACK(guanxx_button_press_event),
                                     G_CALLBACK(guanxx_enter_notify_event),
                                     G_CALLBACK(guanxx_leave_notify_event),
                                     G_CALLBACK(guanxx_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Guanxx_event_box, 509, 0);
    infotv();
    gtk_widget_show_all(Infowind);
}