#include <unistd.h>
#include <logger.h>
#include "test.h"

int sendfd, recvfd;

int main()
{
    int pipefd[2];
    pipe(pipefd);
    recvfd = pipefd[0];
    sendfd = pipefd[1];

    log_info("PROTOCOL_TEST", "Test Begin");
    status_test();

    return 0;
}