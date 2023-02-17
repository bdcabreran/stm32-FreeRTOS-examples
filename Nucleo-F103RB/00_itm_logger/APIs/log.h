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
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
} LogLevel;

void log_message(LogLevel level, const char* message);
void log_test(void);


#endif