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

uint8_t log_message_set_level(log_level_t level);
log_level_t log_message_get_level(void);
void log_test(void);

#define COLOR_PRINTF(color, format, ...) printf(color format COLOR_RESET, ##__VA_ARGS__)

#define log_message(level, format, ...) \
    do { \
        if (level <= log_message_get_level()) { \
            const char *color = ""; \
            switch (level) \
            { \
            case LOG_LEVEL_ERROR:   color = COLOR_RED;     break; \
            case LOG_LEVEL_WARNING: color = COLOR_YELLOW;  break; \
            case LOG_LEVEL_INFO:    color = COLOR_GREEN;   break; \
            case LOG_LEVEL_DEBUG:   color = COLOR_MAGENTA; break; \
            case LOG_LEVEL_TRACE:   color = COLOR_CYAN;    break; \
            } \
			printf(color);\
			printf(format, ##__VA_ARGS__);\
			printf(COLOR_RESET);\
        } \
    } while (0)


    
#endif
