#pragma once

#include <pthread.h>

/**
* 打印INFO日志
*/
__attribute_format_arg__(format)
void log_info(const char *section, const char *format, ...);

/**
* 打印WARN日志
*/
__attribute_format_arg__(format)
void log_warning(const char *section, const char *format, ...);

/**
* 打印ERROR日志
*/
__attribute_format_arg__(format)
void log_error(const char *section, const char *format, ...);