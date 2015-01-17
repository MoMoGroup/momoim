#include <logger.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

static pthread_mutex_t logger_output_lock = PTHREAD_MUTEX_INITIALIZER;
const char *plainPrefix[] = {
        "[INFO][%s]",
        "[WARN][%s]",
        "[ERROR][%s]"
};
const char *colorPrefix[] = {
        "\033[0m[\033[32mINFO\033[0m][\033[34m%s\033[0m]",
        "\033[0m[\033[31mWARN\033[0m][\033[34m%s\033[0m]",
        "\033[0m[\033[31m\033[47mERROR\033[0m][\033[34m%s\033[0m]"
};
const char **prefix = NULL;

static void log_check_color()
{
    if (!prefix)
    {
        if (isatty(STDERR_FILENO))
        {
            prefix = colorPrefix;
        }
        else
        {
            prefix = plainPrefix;
        }
    }
}

void log_info(const char *section, const char *format, ...)
{
    log_check_color();
    pthread_cleanup_push(pthread_mutex_unlock, &logger_output_lock);
            pthread_mutex_lock(&logger_output_lock);
            fprintf(stderr, prefix[0], section);
            va_list ag;
            va_start(ag, format);
            vfprintf(stderr, format, ag);
            va_end(ag);
            fflush(stderr);
    pthread_cleanup_pop(1);
}

void log_warning(const char *section, const char *format, ...)
{
    log_check_color();
    pthread_cleanup_push(pthread_mutex_unlock, &logger_output_lock);
            pthread_mutex_lock(&logger_output_lock);
            fprintf(stderr, prefix[1], section);
            va_list ag;
            va_start(ag, format);
            vfprintf(stderr, format, ag);
            va_end(ag);
            fflush(stderr);
    pthread_cleanup_pop(1);
}

void log_error(const char *section, const char *format, ...)
{
    log_check_color();
    pthread_cleanup_push(pthread_mutex_unlock, &logger_output_lock);
            pthread_mutex_lock(&logger_output_lock);
            fprintf(stderr, prefix[2], section);
            va_list ag;
            va_start(ag, format);
            vfprintf(stderr, format, ag);
            va_end(ag);
            fflush(stderr);
    pthread_cleanup_pop(1);
}