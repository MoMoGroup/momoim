#pragma once

#include <pthread.h>

extern pthread_mutex_t logger_output_lock;

void log_info(const char *section, const char *format, ...);

void log_warning(const char *section, const char *format, ...);

void log_error(const char *section, const char *format, ...);