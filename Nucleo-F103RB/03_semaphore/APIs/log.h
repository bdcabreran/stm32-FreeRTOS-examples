/**
 * @file log.h
 * @author Bayron Cabrera (bdcabreran@unal.edu.co)
 * @brief  Printf implementation using SWO ITM
 * @version 0.1
 * @date 2023-02-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdint.h>

// Enable/Disable _write overwrite
#define OVERWRITE_PRINTF_WRITE  (0)

// Define color codes
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

// Define log levels
typedef enum {
    LOG_LEVEL_ERROR,   // only errors
    LOG_LEVEL_WARNING, // errors and warnings
    LOG_LEVEL_INFO,    // errors, warnings, and information
    LOG_LEVEL_DEBUG,   // all messages, including debug messages
    LOG_LEVEL_TRACE,   // all messages, including trace messages
} log_level_t;

#define IS_VALID_LOG_LEVEL(level) ((level >= LOG_LEVEL_ERROR) & \
                                   (level <= LOG_LEVEL_TRACE))

uint8_t log_init(log_level_t log_level, int (*_write)(char *ptr, int len));
uint8_t log_set_level(log_level_t level);
void log_demo(void);
log_level_t log_get_level(void);

#define log_message_tag(level, tag, format, ...) \
    do { \
        if (level <= log_get_level()) { \
            const char *color = ""; \
            const char *level_str = ""; \
            switch (level) \
            { \
            case LOG_LEVEL_ERROR:   color = COLOR_RED;     level_str = "E";   break; \
            case LOG_LEVEL_WARNING: color = COLOR_YELLOW;  level_str = "W"; break; \
            case LOG_LEVEL_INFO:    color = COLOR_GREEN;   level_str = "I";    break; \
            case LOG_LEVEL_DEBUG:   color = COLOR_MAGENTA; level_str = "D";   break; \
            case LOG_LEVEL_TRACE:   color = COLOR_CYAN;    level_str = "T";   break; \
            } \
			printf("%s%s (%ld) %s: ", color, level_str, HAL_GetTick(), tag);\
			printf(format, ##__VA_ARGS__);\
			printf(COLOR_RESET);\
        } \
    } while (0)


#define log_message(level, format, ...) log_message_tag(level, "", format, ##__VA_ARGS__)


#endif

