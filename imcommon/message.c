#include <unistd.h>
#include <imcommon/message.h>
#include <malloc.h>
#include <fcntl.h>

int MessageFileClose(MessageFile *file)
{
    int ret = close(file->fd);
    if (ret == 0)
    {
        pthread_mutex_unlock(&file->lock);
        pthread_mutex_destroy(&file->lock);
        free(file);
    }
    return !ret;
}

int MessageFileCreate(const char *path)
{
    int fd = creat(path, 0600);
    if (fd)
    {
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
    int fd = open(path, O_RDWR | O_APPEND | O_CLOEXEC | O_CREAT, 0600);
    if (fd < 0)
    {
        return NULL;
    }
    MessageFile *file = (MessageFile *) malloc(sizeof(MessageFile));
    file->fd = fd;
    pthread_mutex_init(&file->lock, NULL);
    return file;
}

int MessageFileCleanup(MessageFile *file)
{
    return !ftruncate(file->fd, 0);
}

int MessageFileAppend(MessageFile *file, UserMessage *message)
{
    int ret = 0;
    pthread_mutex_lock(&file->lock);
    off_t posCur = lseek(file->fd, 0, SEEK_CUR);
    off_t posEnd = lseek(file->fd, 0, SEEK_END);
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
        goto cleanup;
    }
    lseek(file->fd, posCur, SEEK_SET);
    ret = (UserMessage *) malloc(sizeof(UserMessage) + header.messageLen);
    nbytes = read(file->fd, &header, sizeof(UserMessage) + header.messageLen);
    if (nbytes != sizeof(UserMessage) + header.messageLen)
    {
        free(ret);
        goto cleanup;
    }
    cleanup:
    lseek(file->fd, posCur, SEEK_SET);
    pthread_mutex_unlock(&file->lock);
    return ret;
}