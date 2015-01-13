#include <unistd.h>
#include <imcommon/message.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int MessageFileClose(MessageFile *file)
{
    lseek(file->fd, sizeof(uint32_t), SEEK_SET);
    write(file->fd, &file->lastUpdateDate, sizeof(uint32_t));
    int ret = close(file->fd);
    if (ret == 0)
    {
        pthread_mutex_unlock(&file->lock);
        pthread_mutex_destroy(&file->lock);
        free(file);
    }
    return !ret;
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
    int fd = creat(path, 0600);
    if (fd)
    {
        MessageFileReset(fd);
        close(fd);
        return 1;
    }
    else
    {
        return 0;
    }
}

MessageFile *MessageFileOpen(const char *path)
{
    int fd = open(path, O_RDWR | O_CLOEXEC | O_CREAT, 0600);
    if (fd < 0)
    {
        return NULL;
    }
    MessageFile *file = (MessageFile *) malloc(sizeof(MessageFile));
    if (file == NULL)
    {
        close(fd);
        return NULL;
    }
    file->fd = fd;
    pthread_mutex_init(&file->lock, NULL);

    if (sizeof(uint32_t) == read(fd, &file->fileBeginDate, sizeof(uint32_t)))
    {
        read(fd, &file->lastUpdateDate, sizeof(uint32_t));
    }
    else
    {
        MessageFileReset(fd);
        file->fileBeginDate = file->lastUpdateDate = (uint32_t) (time(NULL) / (24 * 60 * 60));
    }
    file->currentDate = file->fileBeginDate;
    file->currentBeginOffset = file->fileBeginOffset =
            sizeof(file->fileBeginDate)
            + sizeof(file->lastUpdateDate)
            + 10 * 365 * sizeof(off_t);
    if (file->fileBeginOffset != lseek(file->fd, file->fileBeginOffset, SEEK_SET))
    {
        free(file);
        close(fd);
        return NULL;
    }
    return file;
}

int MessageFileAppend(MessageFile *file, UserMessage *message)
{
    int ret = 0;
    uint32_t messageDate = (uint32_t) (message->time / (24 * 60 * 60));
    pthread_mutex_lock(&file->lock);
    off_t posCur = lseek(file->fd, 0, SEEK_CUR);
    off_t posEnd = lseek(file->fd, 0, SEEK_END);
    if (file->lastUpdateDate != messageDate)
    {
        lseek(file->fd,
              sizeof(file->fileBeginDate) +
              sizeof(file->lastUpdateDate) +
              sizeof(off_t) * (messageDate - file->lastUpdateDate + 1),
              SEEK_SET);
        for (int i = file->lastUpdateDate + 1; i <= messageDate; ++i)
        {
            write(file->fd, &posEnd, sizeof(off_t));
        }
        file->lastUpdateDate = messageDate;
        lseek(file->fd, 0, SEEK_END);
    }
    ssize_t nbytes;
    nbytes = write(file->fd, message, sizeof(UserMessage));
    if (nbytes != sizeof(UserMessage))
    {
        ftruncate(file->fd, posEnd);
        goto cleanup;
    }
    nbytes = write(file->fd, message->content, (size_t) message->messageLen);
    if (nbytes != message->messageLen)
    {
        ftruncate(file->fd, posEnd);
        goto cleanup;
    }
    ret = 1;

    cleanup:
    lseek(file->fd, posCur, SEEK_SET);
    pthread_mutex_unlock(&file->lock);
    return ret;
}

UserMessage *MessageFileNext(MessageFile *file)
{
    UserMessage *ret = NULL;
    pthread_mutex_lock(&file->lock);
    off_t posCur = lseek(file->fd, 0, SEEK_CUR);

    UserMessage header;
    ssize_t nbytes = read(file->fd, &header, sizeof(UserMessage));
    if (nbytes != sizeof(UserMessage))
    {
        lseek(file->fd, posCur, SEEK_SET);
        goto cleanup;
    }
    ret = (UserMessage *) malloc(sizeof(UserMessage) + header.messageLen);
    memcpy(ret, &header, sizeof(UserMessage));
    nbytes = read(file->fd, ret->content, header.messageLen);
    if (nbytes != header.messageLen)
    {
        free(ret);
        ret = NULL;
        lseek(file->fd, posCur, SEEK_SET);
        goto cleanup;
    }
    cleanup:
    pthread_mutex_unlock(&file->lock);
    return ret;
}

int MessageFileSeek(MessageFile *file, uint32_t date)
{
    int ret = 0;
    pthread_mutex_lock(&file->lock);

    if (date < file->fileBeginDate)
    {
        ret = file->fileBeginOffset == lseek(file->fd, file->fileBeginOffset, SEEK_SET);
    }
    else if (date > file->lastUpdateDate)
    {
        ret = lseek(file->fd, 0, SEEK_END) != -1;
    }
    else if (date == file->currentDate)
    {
        ret = file->currentBeginOffset == lseek(file->fd, file->currentBeginOffset, SEEK_SET);
    }
    else
    {
        lseek(file->fd, sizeof(file->fileBeginDate) +
                sizeof(file->lastUpdateDate) +
                sizeof(off_t) * (date - file->fileBeginDate), SEEK_SET);
        off_t dateSet;
        if (sizeof(dateSet) != read(file->fd, &dateSet, sizeof(dateSet)))
        {
            lseek(file->fd, file->currentBeginOffset, SEEK_SET);
            goto cleanup;
        }
        file->currentDate = date;
        file->currentBeginOffset = dateSet;
        ret = file->currentBeginOffset == lseek(file->fd, file->currentBeginOffset, SEEK_SET);
    }
    cleanup:
    pthread_mutex_unlock(&file->lock);
    return ret;
}