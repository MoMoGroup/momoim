#include <logger.h>
#include <stdio.h>
#include <stdarg.h>

pthread_mutex_t logger_output_lock = {0};

void log_info(const char *section, const char *format, ...)
{
    pthread_mutex_lock(&logger_output_lock);
    fprintf(stderr, "\033[0m[\033[32mINFO\033[0m][\033[34m%s\033[0m]", section);
    va_list ag;
    va_start(ag, format);
    vfprintf(stderr, format, ag);
    va_end(ag);
    fflush(stderr);
    pthread_mutex_unlock(&logger_output_lock);
}

void log_warning(const char *section, const char *format, ...)
{
    pthread_mutex_lock(&logger_output_lock);
    fprintf(stderr, "\033[0m[\033[31mWARN\033[0m][\033[34m%s\033[0m]", section);
    va_list ag;
    va_start(ag, format);
    vfprintf(stderr, format, ag);
    va_end(ag);
    fflush(stderr);
    pthread_mutex_unlock(&logger_output_lock);
}

void log_error(const char *section, const char *format, ...)
{
    pthread_mutex_lock(&logger_output_lock);
    fprintf(stderr, "\033[0m[\033[31m\033[47mERROR\033[0m][\033[34m%s\033[0m]", section);
    va_list ag;
    va_start(ag, format);
    vfprintf(stderr, format, ag);
    va_end(ag);
    fflush(stderr);
    pthread_mutex_unlock(&logger_output_lock);
}