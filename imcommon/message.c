#include <unistd.h>
#include <imcommon/message.h>
#include <malloc.h>
#include <stdlib.h>

static const char *SQLCreateTable = ""
        "CREATE TABLE msg("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "from INTEGER,"
        "to INTEGER,"
        "time INTEGER,"
        "type INTEGER,"
        "content BLOB"
        ");"
        "CREATE INDEX IDX_MSG_FROM ON msg (from);"
        "CREATE INDEX IDX_MSG_TO ON msg (to);";
static const char *SQLInsertMessage = ""
        "INSERT INTO msg("
        "from,to,time,type,content"
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

static void MessageFileReset(int fd)
{
    lseek(fd, 0, SEEK_SET);
    ftruncate(fd, 0);
    uint32_t startTime = (uint32_t) (time(NULL) / (24 * 60 * 60));
    write(fd, &startTime, sizeof(uint32_t));//fileBeginDate
    write(fd, &startTime, sizeof(uint32_t));//lastUpdateDate
    off_t messageBegin = sizeof(uint32_t) + sizeof(uint32_t) + (10 * 365 * sizeof(off_t));
    write(fd, &messageBegin, sizeof(off_t));//First Message Offset
    lseek(fd, messageBegin - 1, SEEK_SET);
    write(fd, "", 1);
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

int MessageFileQuery(MessageFile *file, MessageQueryCondition *condition)
{
    sqlite3_stmt *stmt;


    return 0;
}
