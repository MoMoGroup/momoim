#include <logger.h>
#include <sys/socket.h>
#include "test.h"
#include <unistd.h>

int sendfd, recvfd;

int main()
{
    int fd[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    recvfd = fd[0];
    sendfd = fd[1];

    log_info("PROTOCOL_TEST", "Test Begin\n");

    info_test();
    status_test();
    message_test(); //函数声明
    login_test();
    fridata_test();

    close(sendfd);
    close(recvfd);
    return 0;
}