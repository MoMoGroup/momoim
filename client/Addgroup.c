#include "Addgroup.h"
#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include<stdlib.h>
#include <protocol/CRPPackets.h>
#include <logger.h>


add_group_Interface_fun()
{
//    GtkWidget *add_group_window,*add_group_framelayout,*add_group_layout;
//    add_group_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//    add_group_framelayout = gtk_layout_new(NULL, NULL);
//    add_group_layout = gtk_fixed_new();
//
//    gtk_window_set_position(GTK_WINDOW(add_group_window), GTK_WIN_POS_CENTER);//窗口位置
//    gtk_window_set_resizable(GTK_WINDOW (add_group_window), FALSE);//固定窗口大小
//    gtk_window_set_decorated(GTK_WINDOW(add_group_window), FALSE);//去掉边框
//    gtk_widget_set_size_request(GTK_WIDGET(add_group_window), 250, 235);
    return 0;

}

int Add_group_button_press_event()
{
//    add_group_Interface_fun();
    //CRPFriendGroupAddSend(sockfd, <#(uint32_t)sessionID#>, <#(uint8_t)gid#>, <#(char const[64])name#>);
    log_info("ADD", "\n");
    return 0;

}