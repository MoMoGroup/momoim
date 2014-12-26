#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <data/auth.h>

sqlite3 *db = 0;

sqlite3_stmt *authStmt;
pthread_mutex_t lockAuthStmt = {0};
char zsql[1000];

int AuthInit()
{
    int ret;
    ret = sqlite3_open("auth.db", &db);
    if (ret != SQLITE_OK)
    {
        return 0;
    }
    const char sql[] = "SELECT id FROM users WHERE name = ? AND key = ?;";
    ret = sqlite3_prepare_v2(db, sql, sizeof(sql), &authStmt, NULL);
    if (ret != SQLITE_OK)
    {
        return 0;
    }
    return 1;
}

void AuthFinalize()
{
    sqlite3_close(db);
}

int AuthUser(const char *user, const unsigned char *hashKey, uint32_t *uid)
{
    int ret = 0;
    char *hashText = malloc(33);//128bit MD5 hash * 2(hex char) + 1(zero char)
    for (int i = 0; i < 16; ++i)
    {
        hashText[i * 2] = (char) ((hashKey[i] >> 4) > 9 ? 'a' + (hashKey[i] >> 4 - 10) : '0' + (hashKey[i] >> 4));
        hashText[i * 2 + 1] = (char) ((hashKey[i] & 0xf) > 9 ? 'a' + (hashKey[i] & 0xf - 10) : '0' + (hashKey[i] & 0xf));
    }
    hashText[32] = 0;
    pthread_mutex_lock(&lockAuthStmt);
    sqlite3_bind_text(authStmt, 1, user, strlen(user), NULL);
    sqlite3_bind_text(authStmt, 2, hashText, 32, NULL);
    int r = sqlite3_step(authStmt);
    if (r != SQLITE_ROW)
    {
        ret = r;
        goto cleanup;
    }
    else
    {
        *uid = (uint32_t) sqlite3_column_int(authStmt, 0);
    }

    cleanup:
    sqlite3_reset(authStmt);
    pthread_mutex_unlock(&lockAuthStmt);
    return ret;
}