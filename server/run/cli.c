#include <run/cli.h>
#include <limits.h>
#include <stdio.h>
#include<stdint.h>
#include <string.h>
#include <stdlib.h>
#include <run/Structures.h>
#include <run/user.h>
#include <arpa/inet.h>
#include <server.h>

static void printMenu()
{
    printf("MoMO服务器:\n"
                   "当前有以下指令可以使用:\n"
                   "kick <uid>   - 将一个用户踢出服务器\n"
                   "ban <ipv4>   - 拒绝一个IP地址连入服务器\n"
                   "unban <ipv4> - 允许一个IP地址连入服务器\n"
                   ":");
}

void CLIKick(const char *arg)
{
    uint32_t uid = (uint32_t) atol(arg);
    POnlineUser user = OnlineUserGet(uid);
    if (user)
    {
        OnlineUserDelete(user);
        printf("用户已踢出服务器\n");
    }
    else
    {
        printf("用户不在线\n");
    }
}

void CLIBan(const char *arg)
{
    struct in_addr ip;
    if (inet_aton(arg, &ip) == 0)
    {
        printf("无效IP地址\n");
    }
    else
    {
        BanListAdd(ip.s_addr);
        printf("已添加到拒绝列表\n");
    }
}

void CLIUnban(const char *arg)
{

    struct in_addr ip;
    if (inet_aton(arg, &ip) == 0)
    {
        printf("无效IP地址\n");
    }else{
        BanListRemove(ip.s_addr);
        printf("已从拒绝列表中移除\n");
    }
}

int CLIHandle()
{
    printMenu();
    char buf[LINE_MAX];
    if (!fgets(buf, LINE_MAX, stdin))
    {
        return 0;
    }
    if (memcmp(buf, "kick ", 5) == 0)
    {
        CLIKick(buf + 5);
    }
    else if (memcmp(buf, "ban ", 4) == 0)
    {
        CLIBan(buf + 4);
    }
    else if (memcmp(buf, "unban ", 6) == 0)
    {
        CLIUnban(buf + 6);
    }
    else
    {
        printf("指令未找到\n");
    }
    return 1;
}
