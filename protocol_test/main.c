#include <logger.h>
#include <sys/socket.h>
#include "test.h"
#include <openssl/md5.h>

CRPContext cs, cr;

int main()
{
    int fd[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

    log_info("PROTOCOL_TEST", "Test Begin\n");

    cs = CRPOpen(fd[1]);
    cr = CRPOpen(fd[0]);
    char sdkey[32], rvkey[32];
    char iv[32];
    MD5("sdkey", 3, sdkey);
    MD5("rvkey", 3, rvkey);
    MD5("iv", 2, iv);
    CRPEncryptEnable(cs, sdkey, rvkey, iv);
    CRPEncryptEnable(cr, rvkey, sdkey, iv);

    info_test();
    status_test();
    message_test();
    login_test();
    fridata_test();
    frirequest_test();
    infodata_test();
    inforequest_test();

    CRPClose(cs);
    CRPClose(cr);
    return 0;
}