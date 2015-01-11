#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include <ftadvanc.h>
#include "chart.h"
#include "common.h"
#include <logger.h>
#include <sys/stat.h>
#include "Infomation.h"
#include <pwd.h>
#include <math.h>
#include <logger.h>
#include <sys/stat.h>
#include <cairo-script-interpreter.h>
#include <imcommon/friends.h>

#include "chartmessage.h"

//解码
void DecodingText(const gchar *text, FriendInfo *info, int count)
{

    gchar *ptext = text, *ptext_end = text + count;
    GtkTextBuffer *show_buffer;
    GtkTextIter start, end;
    GtkTextTag *wordtag;
    show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (info->show_text));
    gtk_text_buffer_get_bounds(show_buffer, &start, &end);
    wordtag = gtk_text_buffer_create_tag(show_buffer, NULL, NULL, NULL);
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
                    g_object_set(wordtag, "font", ptext + 2, NULL);
                    ptext = ptext + i + 1;
                    break;

                };

                case 2: //是否斜体
                {
                    ptext = ptext + 2;
                    int style_value = *ptext;
                    if (style_value == 1)
                    {
                        g_object_set(wordtag, "style", PANGO_STYLE_ITALIC, NULL);
                    }
                    ptext++;
                    break;
                };
                case 3:   //宽度
                {
                    ptext = ptext + 2;
                    int weight_value = 0;
                    memcpy(&weight_value, ptext, 2);
                    g_object_set(wordtag, "weight", weight_value, NULL);
                    ptext = ptext + 2;
                    break;
                };
                case 4: //字体大小
                {
                    ptext = ptext + 2;
                    gint size_value;
                    size_value = *ptext;
                    g_object_set(wordtag, "size", size_value * 1024, NULL);
                    g_object_set(wordtag, "size-set", 1, NULL);
                    ptext++;
                    break;
                };
                case 5:
                {
                    ptext = ptext + 2;
                    guint16 colorred;
                    guint16 colorgreen;
                    guint16 colorblue;
                    GdkRGBA rgba;
                    memcpy(&colorred, ptext, 2);
                    memcpy(&colorgreen, ptext + 2, 2);
                    memcpy(&colorblue, ptext + 4, 2);
                    g_print("the red is %u\n", colorred);
                    g_print("the green is %u\n", colorgreen);
                    g_print("the blue is %u\n", colorblue);
                    rgba.alpha = 1;
                    rgba.red = colorred / 65535.0;
                    rgba.green = colorgreen / 65535.0;
                    rgba.blue = colorblue / 65535.0;
                    g_print("the red rgba is %lf\n", rgba.red);
                    g_print("the green rgba is %lf\n", rgba.green);
                    g_print("the blue is rgba %lf\n", rgba.blue);
                    g_object_set(wordtag, "foreground-rgba", &rgba, NULL);
                    ptext = ptext + 6;
                    break;
                }
                case 0:
                {
                    GtkTextChildAnchor *anchor;
                    GtkWidget *image;
                    char filename[256] = {0};
                    char strdest[16] = {0};
                    ptext = ptext + 2;
                    memcpy(strdest, ptext, 16);
                    HexadecimalConversion(filename, strdest); //进制转换，将MD5值的字节流转换成十六进制
                    anchor = gtk_text_buffer_create_child_anchor(show_buffer, &end);
                    image = gtk_image_new_from_file(filename);
                    gtk_widget_show_all(image);
                    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW (info->show_text), image, anchor);
                    ptext = ptext + 16;
                    break;
                }
                default:
                    break;
            }


        }
        else
        {
            gchar *next_char = g_utf8_next_char(ptext);
            gtk_text_buffer_insert_with_tags(show_buffer, &end, ptext, next_char - ptext, wordtag, NULL);
            ptext = next_char;
        }

    }
    gtk_text_buffer_insert_with_tags_by_name(show_buffer, &end,
                                             "\n", -1, "gray_foreground", NULL);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW (info->show_text),
                                       gtk_text_buffer_get_insert(info->show_buffer));//实现自动滚屏的效果
}

//将输入的文本框输出在显示的文本框中
void show_local_text(const gchar *text, FriendInfo *info, char *nicheng_times, int count)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(info->show_buffer, &start, &end);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &end,
                                             nicheng_times, -1, "red_foreground", "size1", NULL);
    DecodingText(text, info, count); //解码

}


//将服务器发过来的的消息显示在文本框上
void ShoweRmoteText(const gchar *rcvd_text, FriendInfo *info, uint16_t len)
{
    GtkTextIter start, end;
    GtkTextBuffer *show_buffer;
    char nicheng_times[40] = {0};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(nicheng_times, " %s  %d: %d: %d \n", CurrentUserInfo->nickName, p->tm_hour, p->tm_min, p->tm_sec);
    show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (info->show_text));
    gtk_text_buffer_get_bounds(show_buffer, &start, &end);
    gtk_text_buffer_insert_with_tags_by_name(show_buffer, &end,
                                             nicheng_times, -1, "blue_foreground", "size1", NULL);
    DecodingText(rcvd_text, info, len); //解码

}

//编码字体和颜色
void CodingWordColor(FriendInfo *info, gchar *coding, int *count)
{
    gchar *char_rear = coding;
    //字体类型

    size_t fontcount;
    char_rear[0] = '\0';
    char_rear[1] = 1;
    char_rear = char_rear + 2;
    fontcount = strlen(UserWordInfo.font);
    memcpy(char_rear, UserWordInfo.font, fontcount);
    char_rear = char_rear + fontcount;
    char_rear[0] = '\0';
    char_rear++;

    //字体是否斜体
    char_rear[0] = '\0';
    char_rear[1] = 2;
    char_rear = char_rear + 2;
    *char_rear = UserWordInfo.style == PANGO_STYLE_ITALIC;
    char_rear++;

    //字体宽度
    char_rear[0] = '\0';
    char_rear[1] = 3;
    char_rear = char_rear + 2;
    int weight_value = UserWordInfo.weight;
    memcpy(char_rear, &weight_value, 2);
    char_rear = char_rear + 2;
    //字体大小
    char_rear[0] = '\0';
    char_rear[1] = 4;
    char_rear = char_rear + 2;
    memcpy(char_rear, &UserWordInfo.size, 1);
    char_rear = char_rear + 1;
    //字体颜色
    char_rear[0] = '\0';
    char_rear[1] = 5;
    char_rear = char_rear + 2;
    memcpy(char_rear, &UserWordInfo.color_red, 2);

    memcpy(char_rear + 2, &UserWordInfo.color_green, 2);
    memcpy(char_rear + 4, &UserWordInfo.color_blue, 2);
    char_rear = char_rear + 6;
    *count = char_rear - coding;
}


//编码
void CodingTextImage(FriendInfo *info, gchar *coding, int *count)
{
    gchar *char_rear = coding + *count;
    gunichar c;

    GtkTextIter iter;
    GtkTextChildAnchor *anchor;
    int num;

    gtk_text_buffer_get_start_iter(info->input_buffer, &iter);

    //编码字符串和图片
    while (!gtk_text_iter_is_end(&iter))
    {
        anchor = gtk_text_iter_get_child_anchor(&iter);
        if (anchor == NULL)
        {

            c = gtk_text_iter_get_char(&iter);
            num = g_unichar_to_utf8(c, char_rear);
            char_rear = char_rear + num;
        }
        else
        {
            char_rear[0] = 0;
            char_rear[1] = 0;
            char_rear = char_rear + 2;
            GList *list = gtk_text_child_anchor_get_widgets(anchor);
            GtkWidget *imageWidget = g_list_nth_data(list, 0);
            g_list_free(list);
            gchar *src = g_object_get_data(imageWidget, "ImageSrc");
            Md5Coding(src, char_rear);
            char targetfilename[256] = {0};
            HexadecimalConversion(targetfilename, char_rear); //进制转换，将MD5值的字节流转换成十六进制
            CopyFile(src, targetfilename);
            char_rear = char_rear + MD5_DIGEST_LENGTH;
        }
        gtk_text_iter_forward_char(&iter);
    }
    *count = char_rear - coding;

}

int deal_with_message(CRPBaseHeader *header, void *data)
{
    struct PictureMessageFileUploadingData *photomessage = (struct PictureMessageFileUploadingData *) data;
    char *imagedata = (char *) malloc(4096);
    size_t num;
    int ret = 1;
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        CRPPacketFailure *infodata = CRPFailureCast(header);
        log_info("FAILURE reason", infodata->reason);
        fclose(photomessage->fp);
        free(photomessage);
        free(imagedata);
        return 0;
    }
    log_info("Message", "Packet id :%d,SessionID:%d\n", header->packetID, header->sessionID);

    if (header->packetID == CRP_PACKET_OK)
    {
        if (photomessage->fp != NULL)
        {

            num = fread(imagedata, sizeof(char), 4096, photomessage->fp);
            if (num > 0)
            {
                CRPFileDataSend(sockfd, header->sessionID, num, photomessage->seq, imagedata);
                photomessage->seq++;
            }
            else
            {
                fclose(photomessage->fp);
                CRPFileDataEndSend(sockfd, header->sessionID, FEC_OK);
                photomessage->image_message_data->imagecount--;
                if (photomessage->image_message_data->imagecount == 0)
                {
                    session_id_t sessionID = CountSessionId();
                    CRPMessageNormalSend(sockfd, sessionID, UMT_TEXT,
                                         photomessage->image_message_data->uid,
                                         photomessage->image_message_data->charlen,
                                         photomessage->image_message_data->message_data);
                    free(photomessage->image_message_data->message_data);
                    free(photomessage->image_message_data);
                    ret = 0;
                }
                free(photomessage);
            }

        }
    }
    else if (header->packetID == CRP_PACKET_FILE_DATA_END)
    {
        fclose(photomessage->fp);
        photomessage->image_message_data->imagecount--;
        if (photomessage->image_message_data->imagecount == 0)
        {
            CRPMessageNormalSend(sockfd, photomessage->image_message_data->uid, UMT_TEXT,
                                 photomessage->image_message_data->uid, photomessage->image_message_data->charlen,
                                 photomessage->image_message_data->message_data);
            free(photomessage->image_message_data->message_data);
            free(photomessage->image_message_data);
        }
        free(photomessage);
        ret = 0;
    }

    free(imagedata);
    return ret;
}

int image_message_send(gchar *char_text, FriendInfo *info, int charlen)
{
    int i = 0;
    int isimageflag = 0;

    struct ImageMessageFileData *image_message_data_state
            = (struct ImageMessageFileData *) malloc(sizeof(struct ImageMessageFileData));
    image_message_data_state->imagecount = 0;
    image_message_data_state->message_data = char_text;
    image_message_data_state->uid = info->user.uid;
    image_message_data_state->charlen = charlen;
    while (i < charlen)
    {
        if (char_text[i] != '\0')
        {
            i++;
        }
        else
        {

            switch (char_text[i + 1])
            {
                case 1:
                {
                    i++;
                    while (char_text[i] != '\0')
                    {
                        i++;
                    }
                    i++;
                    break;
                };
                case 2 :
                {
                    i += 3;
                };
                case 3:   //宽度
                {
                    i += 4;
                    break;
                };
                case 4: //字体大小
                {
                    i += 2;
                    break;
                };
                case 5: //颜色
                {
                    i += 8;

                    break;
                };
                case 0 :
                {
                    isimageflag = 1;
                    char filename[256] = {0};
                    char strdest[17] = {0};
                    struct stat stat_buf;
                    session_id_t session_id;
                    struct PictureMessageFileUploadingData *imagemessge
                            = (struct PictureMessageFileUploadingData *) malloc(sizeof(struct PictureMessageFileUploadingData));
                    i += 2;
                    memcpy(strdest, &char_text[i], 16);
                    HexadecimalConversion(filename, strdest); //进制转换，将MD5值的字节流转换成十六进制
                    stat(filename, &stat_buf);
                    session_id = CountSessionId();
                    imagemessge->seq = 0;
                    imagemessge->fp = (fopen(filename, "r"));
                    imagemessge->image_message_data = image_message_data_state;
                    AddMessageNode(session_id, deal_with_message, imagemessge);
                    CRPFileStoreRequestSend(sockfd, session_id, stat_buf.st_size, 0, char_text + i);
                    imagemessge->image_message_data->imagecount++;
                    i = i + 16;
                };
                default:
                {
//                        i += 2;
                    break;
                };
            }

        }

    }

    if (isimageflag == 0)
    {
        CRPMessageNormalSend(sockfd, info->user.uid, UMT_TEXT, info->user.uid, charlen, char_text);
        free(char_text);
        free(image_message_data_state);
        return 0;
    }
    return 1;
}

//将输入的内容添加到输入文本框的缓冲区去并取出内容传给显示文本框
void SendText(FriendInfo *info)
{
    gchar *char_text;
    int count = 0, cmpcount = 0;
    char_text = (gchar *) malloc(100000);

    if (char_text == NULL)
    {
        printf("Malloc error!\n");
        exit(1);
    }
    // CodingWordColor(info, char_text, &count); //字体编码
    if (UserWordInfo.coding_font_color != NULL)
    {
        memcpy(char_text, UserWordInfo.coding_font_color, UserWordInfo.codinglen);
        count = UserWordInfo.codinglen;
    }
    else
    {
        CodingWordColor(info, char_text, &count); //字体编码
    }
    cmpcount = count;
    CodingTextImage(info, char_text, &count); //字符串和图片编码
    if (count > cmpcount)        //避免无内容的发送
    {
        char nicheng_times[40] = {0};
        time_t timep;
        struct tm *p;
        time(&timep);
        p = localtime(&timep);
        sprintf(nicheng_times, " %s  %d : %d: %d \n", CurrentUserInfo->nickName, p->tm_hour, p->tm_min, p->tm_sec);
        gtk_text_buffer_set_text(info->input_buffer, "", 0);//发送消息后本地的文本框清0
        show_local_text(char_text, info, nicheng_times, count);
        image_message_send(char_text, info, count);
    }
}
