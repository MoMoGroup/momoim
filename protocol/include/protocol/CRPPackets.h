#pragma once

#include <stddef.h>
#include "protocol/base.h"
#include "protocol/friend/Request.h"
#include "protocol/friend/Data.h"
#include "protocol/friend/SearchByNickname.h"
#include "protocol/friend/UserList.h"
#include "protocol/friend/Notify.h"
#include "protocol/friend/Add.h"
#include "protocol/friend/Accept.h"
#include "protocol/friend/Move.h"
#include "protocol/friend/Delete.h"
#include "protocol/friend/GroupAdd.h"
#include "protocol/friend/GroupRename.h"
#include "protocol/friend/GroupDelete.h"
#include "protocol/friend/GroupMove.h"

#include "protocol/info/Request.h"
#include "protocol/info/Data.h"
#include "protocol/info/StatusChange.h"
#include "protocol/info/PasswordChange.h"

#include "protocol/file/Request.h"
#include "protocol/file/Data.h"
#include "protocol/file/DataStart.h"
#include "protocol/file/DataEnd.h"
#include "protocol/file/StoreRequest.h"
#include "protocol/file/Reset.h"
#include "protocol/file/Proxy.h"

#include "protocol/login/Login.h"
#include "protocol/login/Accept.h"
#include "protocol/login/Logout.h"
#include "protocol/login/Register.h"

#include "protocol/status/Hello.h"
#include "protocol/status/KeepAlive.h"
#include "protocol/status/OK.h"
#include "protocol/status/Failure.h"
#include "protocol/status/Kick.h"
#include "protocol/status/Crash.h"
#include "protocol/status/Cancel.h"
#include "protocol/status/SwitchProtocol.h"

#include "protocol/message/Normal.h"
#include "protocol/message/QueryOffline.h"
#include "protocol/message/RecordNext.h"
#include "protocol/message/RecordSeek.h"
#include "protocol/message/RecordData.h"

#include "protocol/net/NATDiscover.h"
#include "protocol/net/FriendDiscover.h"
#include "protocol/net/DiscoverAccept.h"
#include "protocol/net/DiscoverRefuse.h"
#include "protocol/net/QualityTest.h"
#include "protocol/net/Detected.h"
#include "protocol/net/InetAddress.h"

typedef enum
{
    CRP_PACKET_KEEP_ALIVE = 0,      //心跳包
    CRP_PACKET_HELLO,               //Hello包
    CRP_PACKET_FAILURE,             //通用失败包
    CRP_PACKET_OK,                  //通用接受包
    CRP_PACKET_KICK,                //被服务器踢出
    CRP_PACKET_CANCEL,              //取消操作
    CRP_PACKET_SWITCH_PROTOCOL,     //切换协议
    CRP_PACKET_CRASH = UINT16_MAX - 1,//崩溃包


    CRP_PACKET_LOGIN__START = 0x10, //登陆类数据包开始
    CRP_PACKET_LOGIN_LOGIN,         //登陆
    CRP_PACKET_LOGIN_ACCEPT,        //接受登陆
    CRP_PACKET_LOGIN_LOGOUT,        //登出
    CRP_PACKET_LOGIN_REGISTER,      //注册请求

    CRP_PACKET_INFO__START = 0x20,
    CRP_PACKET_INFO_REQUEST,        //用户资料请求报
    CRP_PACKET_INFO_DATA,           //用户资料回复
    CRP_PACKET_INFO_STATUS_CHANGE,  //用户资料回复
    CRP_PACKET_INFO_PASSWORD_CHANGE,//密码修改

    CRP_PACKET_FRIEND__START = 0x30,
    CRP_PACKET_FRIEND_REQUEST,      //请求好友列表
    CRP_PACKET_FRIEND_DATA,         //答复好友列表
    CRP_PACKET_FRIEND_NOTIFY,       //好友通知
    CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME, //通过昵称查找好友
    CRP_PACKET_FRIEND_USER_LIST,    //用户列表
    CRP_PACKET_FRIEND_ADD,          //添加好友请求
    CRP_PACKET_FRIEND_ACCEPT,       //同意好友添加请求
    CRP_PACKET_FRIEND_MOVE,         //好友分组请求
    CRP_PACKET_FRIEND_DELETE,       //删除好友请求
    CRP_PACKET_FRIEND_GROUP_ADD,    //添加分组
    CRP_PACKET_FRIEND_GROUP_RENAME, //重命名分组
    CRP_PACKET_FRIEND_GROUP_DELETE, //删除分组
    CRP_PACKET_FRIEND_GROUP_MOVE,   //移动分组

    CRP_PACKET_FILE__START = 0x40, //文件请求类数据包开始
    CRP_PACKET_FILE_REQUEST,       //请求文件
    CRP_PACKET_FILE_DATA,          //响应数据
    CRP_PACKET_FILE_DATA_START,    //响应数据结束
    CRP_PACKET_FILE_DATA_END,      //响应数据结束
    CRP_PACKET_FILE_RESET,         //重置文件发送进程
    CRP_PACKET_FILE_STORE_REQUEST, //请求存储新文件
    CRP_PACKET_FILE_PROXY,         //请求实时文件数据代理

    CRP_PACKET_MESSAGE__START = 0x50, //消息类数据包开始
    CRP_PACKET_MESSAGE_NORMAL,        //消息
    CRP_PACKET_MESSAGE_QUERY_OFFLINE, //查询离线消息
    CRP_PACKET_MESSAGE_RECORD_NEXT, //消息记录向前
    CRP_PACKET_MESSAGE_RECORD_SEEK, //消息记录重定位
    CRP_PACKET_MESSAGE_RECORD_DATA, //消息记录数据

    CRP_PACKET_NET__START = 0x60,   //NAT内网穿透开始
    CRP_PACKET_NET_QUALITY_TEST,    //网络质量测试
    CRP_PACKET_NET_INET_ADDRESS,    //IP地址数据包
    CRP_PACKET_NET_DETECTED,        //主机已检测到
    CRP_PACKET_NET_NAT_DISCOVER,    //NAT网络发现
    CRP_PACKET_NET_FRIEND_DISCOVER, //好友网络发现
    CRP_PACKET_NET_DISCOVER_ACCEPT, //同意被发现
    CRP_PACKET_NET_DISCOVER_REFUSE, //拒绝被发现

    CRP_PACKET_ID_MAX = UINT16_MAX  //最大包ID
} CRPPacketIDs;

//数据转换函数映射数组
extern void *(*const PacketsDataCastMap[CRP_PACKET_ID_MAX + 1])(CRPBaseHeader *base);
