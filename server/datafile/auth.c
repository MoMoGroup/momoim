#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "datafile/auth.h"

static sqlite3 *db = 0;
static const char sqlAuth[] = "SELECT id FROM users WHERE name = ? AND key = ?;";
static const char sqlReg[] = "INSERT INTO users (name,key) VALUES ( ? , ? );";
static const char sqlPwd[] = "UPDATE users SET key = ? WHERE id = ? AND key = ? LIMIT 1;";

int AuthInit()
{
    int ret;
    ret = sqlite3_open("auth.db", &db);
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
    char hashText[33];//128bit MD5 hash * 2(hex char) + 1(null char)
    for (int i = 0; i < 16; ++i)
    {
        hashText[i * 2] = (char) ((hashKey[i] >> 4) > 9 ? 'a' + ((hashKey[i] >> 4) - 10) : '0' + (hashKey[i] >> 4));
        hashText[i * 2 + 1] = (char) ((hashKey[i] & 0xf) > 9 ? 'a' + ((hashKey[i] & 0xf) - 10) : '0' + (hashKey[i] & 0xf));
    }
    hashText[32] = 0;

    sqlite3_stmt *authStmt;
    if (sqlite3_prepare_v2(db, sqlAuth, sizeof(sqlAuth), &authStmt, NULL) != SQLITE_OK)
    {
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
    return ret;
}

int AuthPasswordChange(uint32_t uid, const unsigned char *oldKey, const unsigned char *newKey)
{

    int ret = -1;
    char oldText[33], newText[33];//128bit MD5 hash * 2(hex char) + 1(null char)
    for (int i = 0; i < 16; ++i)
    {
        register char byteBuf;
        byteBuf = oldKey[i] >> 4;
        oldText[i * 2] = (char) (byteBuf > 9 ? 'a' + (byteBuf - 10) : '0' + byteBuf);
        byteBuf = (char) (oldKey[i] & 0xf);
        oldText[i * 2 + 1] = (char) (byteBuf > 9 ? 'a' + (byteBuf - 10) : '0' + byteBuf);

        byteBuf = newKey[i] >> 4;
        newText[i * 2] = (char) (byteBuf > 9 ? 'a' + (byteBuf - 10) : '0' + byteBuf);
        byteBuf = (char) (newKey[i] & 0xf);
        newText[i * 2 + 1] = (char) (byteBuf > 9 ? 'a' + (byteBuf - 10) : '0' + byteBuf);
    }
    oldText[32] = 0;
    newText[32] = 0;
    sqlite3_stmt *cpStmt;
    if (sqlite3_prepare_v2(db, sqlPwd, sizeof(sqlPwd), &cpStmt, NULL) != SQLITE_OK)
    {
        return -1;
    }
    sqlite3_bind_text(cpStmt, 1, newText, 32, NULL);
    sqlite3_bind_int(cpStmt, 2, uid);
    sqlite3_bind_text(cpStmt, 3, oldText, 32, NULL);
    sqlite3_mutex *mutex = sqlite3_db_mutex(db);
    sqlite3_mutex_enter(mutex);//SQLITE3有递归锁,这里不会锁死
    int r = sqlite3_step(cpStmt);
    int chg = sqlite3_changes(db);
    sqlite3_mutex_leave(mutex);
    if (r == SQLITE_DONE)
    {
        ret = chg;
    }
    sqlite3_finalize(cpStmt);
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