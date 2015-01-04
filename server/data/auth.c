#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <data/auth.h>
#include <pthread.h>

static sqlite3 *db = 0;
static const char sqlAuth[] = "SELECT id FROM users WHERE name = ? AND key = ?;";
static const char sqlReg[] = "INSERT INTO users (name,key) VALUES ( ? , ? );";

//static pthread_mutex_t lock;        //认证模块单线程执行
int AuthInit()
{
    int ret;
    ret = sqlite3_open("auth.db", &db);
    if (ret != SQLITE_OK)
    {
        return 0;
    }
    //pthread_mutex_init(&lock, NULL);
    return 1;
}

void AuthFinalize()
{
    sqlite3_close(db);
    //pthread_mutex_destroy(&lock);
}

int AuthUser(const char *user, const unsigned char *hashKey, uint32_t *uid)
{
    int ret = 0;
    char hashText[33];//128bit MD5 hash * 2(hex char) + 1(null char)
    for (int i = 0; i < 16; ++i)
    {
        hashText[i * 2] = (char) ((hashKey[i] >> 4) > 9 ? 'a' + ((hashKey[i] >> 4) - 10) : '0' + (hashKey[i] >> 4));
        hashText[i * 2 + 1] = (char) ((hashKey[i] & 0xf) > 9 ? 'a' + ((hashKey[i] & 0xf) - 10) : '0' + (hashKey[i] & 0xf));
    }
    hashText[32] = 0;

    //pthread_mutex_lock(&lock);
    sqlite3_stmt *authStmt;
    if (sqlite3_prepare_v2(db, sqlAuth, sizeof(sqlAuth), &authStmt, NULL) != SQLITE_OK)
    {
        //pthread_mutex_unlock(&lock);
        return 0;
    }
    sqlite3_bind_text(authStmt, 1, user, (int) strlen(user), NULL);
    sqlite3_bind_text(authStmt, 2, hashText, 32, NULL);
    int r = sqlite3_step(authStmt);
    if (r == SQLITE_ROW)
    {
        ret = 1;
        *uid = (uint32_t) sqlite3_column_int(authStmt, 0);
    }
    sqlite3_finalize(authStmt);
    //pthread_mutex_unlock(&lock);
    return ret;
}

uint32_t AuthRegister(const char *user, const unsigned char *hashKey)
{
    uint32_t ret = 0;
    char hashText[33];//128bit MD5 hash * 2(hex char) + 1(null char)
    for (int i = 0; i < 16; ++i)
    {
        hashText[i * 2] = (char) ((hashKey[i] >> 4) > 9 ? 'a' + ((hashKey[i] >> 4) - 10) : '0' + (hashKey[i] >> 4));
        hashText[i * 2 + 1] = (char) ((hashKey[i] & 0xf) > 9 ? 'a' + ((hashKey[i] & 0xf) - 10) : '0' + (hashKey[i] & 0xf));
    }
    hashText[32] = 0;
    sqlite3_stmt *regStmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, sqlReg, sizeof(sqlReg), &regStmt, NULL))
    {
        //pthread_mutex_unlock(&lock);
        return 0;
    }
    sqlite3_bind_text(regStmt, 1, user, (int) strlen(user), NULL);
    sqlite3_bind_text(regStmt, 2, hashText, 32, NULL);
    int r = sqlite3_step(regStmt);
    if (r == SQLITE_DONE)
    {
        ret = (uint32_t) sqlite3_last_insert_rowid(db);
    }
    sqlite3_finalize(regStmt);
    return ret;
}