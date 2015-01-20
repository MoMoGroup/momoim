#include <run/cli.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <run/Structures.h>
#include <run/user.h>
#include <arpa/inet.h>
#include <server.h>
#include <ctype.h>

//
const char *commands[] = {
        "list",
        "kick",
        "ban",
        "unban",
        "stop"
};
const char *commandDesc[] = {
        "List all online users",
        "Kick out user",
        "Forbid an host connect to server",
        "Allow an host connect to server",
        "Safely stop server"
};
const char *commandArgs[] = {
        "",
        "<uid>",
        "<ipv4>",
        "<ipv4>",
        ""
};


void (*const commandFunc[])(const char *) = {
        CLIList,
        CLIKick,
        CLIBan,
        CLIUnban,
        CLIStop
};

static void printMenu()
{
    printf("MoMO Server:\n");
    for (int i = 0; i < sizeof(commands) / sizeof(*commands); ++i)
    {
        printf("%s %s - %s\n", commands[i], commandArgs[i], commandDesc[i]);
    }
    printf(":");
}

void CLIStop(const char *arg)
{
    IsServerRunning = 0;
}

void CLIKick(const char *arg)
{
    uint32_t uid = (uint32_t) atol(arg);
    POnlineUser user = OnlineUserGet(uid);
    if (user)
    {
        OnlineUserDelete(user);
        printf("The user has been kicked\n");
    }
    else
    {
        printf("User not found.\n");
    }
}

void CLIBan(const char *arg)
{
    struct in_addr ip;
    if (inet_aton(arg, &ip) == 0)
    {
        printf("Invalid ipv4 address\n");
    }
    else
    {
        BanListAdd(ip.s_addr);
        printf("OK\n");
    }
}

void CLIUnban(const char *arg)
{

    struct in_addr ip;
    if (inet_aton(arg, &ip) == 0)
    {
        printf("Invalid ipv4 address\n");
    }
    else
    {
        BanListRemove(ip.s_addr);
        printf("OK\n");
    }
}

void CLIList(const char *arg)
{
    printf("Online User List:\n");
    UserManagerListOnline();
}

int CLIHandle()
{
    printMenu();
    char *buf = (char *) malloc(2048);
    if (!fgets(buf, 2048, stdin))
    {
        return 0;
    }
    if (isspace(*buf))
    {
        return 1;
    }
    for (int i = 0; i < sizeof(commands) / sizeof(*commands); ++i)
    {
        size_t nlen = strlen(commands[i]);
        if (memcmp(buf, commands[i], nlen) == 0)
        {
            if (buf[nlen] == 0)
            {
                commandFunc[i](buf + nlen);
            }
            else if (isspace(buf[nlen]))
            {
                commandFunc[i](buf + nlen + 1);
            }
            else
            {
                continue;
            }
            free(buf);
            return 1;
        }
    }
    printf("Command not found\n");
    free(buf);
    return 0;
}
