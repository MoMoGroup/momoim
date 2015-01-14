#include <unistd.h>
#include <imcommon/message.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "../logger/include/logger.h"

static const char *SQLCreateTable = ""
        "CREATE TABLE msg("
        "`id` INTEGER PRIMARY KEY AUTOINCREMENT,"
        "`from` INTEGER,"
        "`to` INTEGER,"
        "`time` INTEGER,"
        "`type` INTEGER,"
        "`content` BLOB"
        ");"
        "CREATE INDEX IDX_MSG_FROM ON msg (`from`);"
        "CREATE INDEX IDX_MSG_TO ON msg (`to`);";
static const char *SQLInsertMessage = ""
        "INSERT INTO msg("
        "`from`,`to`,`time`,`type`,`content`"
        ") VALUES ("
        "? , ? , ? , ? , ?"
        ");";

int MessageFileClose(MessageFile *file)
{
    int rc = sqlite3_close(file->db);

    if (rc == SQLITE_OK)
    {
        free(file);
        return 1;
    }
    return 0;
}

int MessageFileCreate(const char *path)
{
    sqlite3 *db;
    int rc = sqlite3_open_v2(path, &db, SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (rc == SQLITE_OK)
    {
        rc = sqlite3_exec(db, SQLCreateTable, NULL, NULL, NULL);
        sqlite3_close(db);
        if (rc == SQLITE_OK)
        {
            return 1;
        }
    }
    return 0;
}

MessageFile *MessageFileOpen(const char *path)
{
    sqlite3 *db;
    int rc = sqlite3_open_v2(path, &db, SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK)
    {
        return NULL;
    }
    MessageFile *file = (MessageFile *) malloc(sizeof(MessageFile));
    if (file == NULL)
    {
        sqlite3_close(db);
        return NULL;
    }
    file->db = db;
    file->mutex = sqlite3_db_mutex(db);
    return file;
}

int64_t MessageFileInsert(MessageFile *file, UserMessage *message)
{
    sqlite3_stmt *stmt;
    int64_t ret = -1;
    int rc = sqlite3_prepare_v2(file->db, SQLInsertMessage, sizeof(SQLInsertMessage), &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        return 0;
    }
    sqlite3_bind_int64(stmt, 1, message->from);
    sqlite3_bind_int64(stmt, 2, message->to);
    sqlite3_bind_int64(stmt, 3, message->time);
    sqlite3_bind_int(stmt, 4, message->messageType);
    sqlite3_bind_blob(stmt, 5, message->content, message->messageType, NULL);
    sqlite3_mutex_enter(file->mutex);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        ret = sqlite3_last_insert_rowid(file->db);
    }
    sqlite3_mutex_leave(file->mutex);
    sqlite3_finalize(stmt);
    return ret;
}

const char *operators[] = {"<", "<=", "=", ">=", ">", "AND", "OR"};

static const char *getOperator(int op)
{
    if (op < -2 || op > 4)
    {
        return NULL;
    }
    else
    {
        return operators[op + 2];
    }
}

static const char *getOrderBy(int op)
{
    if (op <= 0)
    {
        return "DESC";
    }
    else
    {
        return "ASC";
    }
}

UserMessage **MessageFileQuery(MessageFile *file, MessageQueryCondition *condition, int *count)
{
    char zSQLPreBuild[150] = "SELECT * FROM msg WHERE ",
            *zSQLTail = zSQLPreBuild + 24;//24==sizeof("SELECT * FROM msg WHERE ")
    char zSQLOrder[100] = "",
            *orderTail = zSQLOrder;
    int conditionCount = 0;
    if (condition->id != -1)
    {
        zSQLTail += sprintf(zSQLTail, "`id` %s %ld ", getOperator(condition->idOperator), condition->id);
        orderTail += sprintf(orderTail, "`id` %s", getOrderBy(condition->idOperator));
        ++conditionCount;
    }
    if (condition->time != -1)
    {
        if (conditionCount)
        {
            zSQLTail += sprintf(zSQLTail, "AND ");
        }
        zSQLTail += sprintf(zSQLTail, "`time` %s %ld ", getOperator(condition->timeOperator), condition->time);
        if (zSQLOrder[0])
        {
            *orderTail = ',';
            ++orderTail;
        }
        orderTail += sprintf(orderTail, "`time` %s", getOrderBy(condition->timeOperator));

        ++conditionCount;
    }
    if (condition->from != 0 && condition->to != 0)
    {
        if (conditionCount)
        {
            zSQLTail += sprintf(zSQLTail, "AND ");
        }
        if (condition->fromtoOperator != 3)
        {
            condition->fromtoOperator = 4;
        }
        zSQLTail += sprintf(zSQLTail,
                            "(`from` = %u %s `to` = %u) ",
                            condition->from,
                            getOperator(condition->fromtoOperator),
                            condition->to);
        ++conditionCount;
    }
    else
    {
        if (condition->from)
        {
            if (conditionCount)
            {
                zSQLTail += sprintf(zSQLTail, "AND ");
            }
            zSQLTail += sprintf(zSQLTail, "`from` = %u ", condition->from);
            ++conditionCount;
        }
        if (condition->to)
        {
            if (conditionCount)
            {
                zSQLTail += sprintf(zSQLTail, "AND ");
            }
            zSQLTail += sprintf(zSQLTail, "`to` = %u ", condition->to);
            ++conditionCount;
        }
    }

    if (condition->messageType != 255)
    {
        if (conditionCount)
        {
            zSQLTail += sprintf(zSQLTail, "AND ");
        }
        zSQLTail += sprintf(zSQLTail, "`type` = %u ", (uint) condition->messageType);
        ++conditionCount;
    }
    if (condition->limit == 0)
    {
        condition->limit = 20;
    }
    if (!conditionCount)    //不允许无任何筛选条件
    {
        return 0;
    }
    if (zSQLOrder[0] != 0)
    {
        zSQLTail += sprintf(zSQLTail, "ORDER BY %s ", zSQLOrder);
    }
    zSQLTail += sprintf(zSQLTail, "LIMIT %d;", (int) condition->limit);
    sqlite3_stmt *stmt;
    log_info("DEBUG", "%s\n", zSQLPreBuild);
    return NULL;
    if (SQLITE_OK != sqlite3_prepare_v2(file->db, zSQLPreBuild, (int) (zSQLTail - zSQLPreBuild), &stmt, NULL))
    {
        return 0;
    }

    int rc;
    UserMessage **messages = (UserMessage **) malloc(sizeof(UserMessage *) * condition->limit),
            *peekedMessage;
    int i = 0;
    int len;
    const char *p;
    while (SQLITE_ROW == (rc = sqlite3_step(stmt)))
    {
        peekedMessage = messages[i];
        len = sqlite3_column_bytes(stmt, 5);
        messages[i] = (UserMessage *) malloc(sizeof(UserMessage) + len);
        peekedMessage->id = sqlite3_column_int64(stmt, 0);
        peekedMessage->from = (uint32_t) sqlite3_column_int64(stmt, 1);
        peekedMessage->to = (uint32_t) sqlite3_column_int64(stmt, 2);
        peekedMessage->time = (time_t) sqlite3_column_int64(stmt, 3);
        peekedMessage->messageType = (uint8_t) sqlite3_column_int(stmt, 4);
        peekedMessage->messageLen = (uint16_t) len;
        p = sqlite3_column_blob(stmt, 5);
        memcpy(peekedMessage->content, p, (size_t) len);
        ++i;
    }
    sqlite3_finalize(stmt);
    *count = i;
    return messages;
}