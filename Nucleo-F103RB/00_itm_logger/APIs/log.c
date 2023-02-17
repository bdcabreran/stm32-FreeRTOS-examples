/**
 * @file printf_dbg.c
 * @author Bayron Cabrera (bdcabreran@unal.edu.co)
 * @brief  Printf implementation using SWO ITM
 * @version 0.1
 * @date 2023-02-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "stdio.h"
#include "log.h"
#include "stm32f1xx.h"

int __io_putchar(int ch)
{
 // Write character to ITM ch.0
 ITM_SendChar(ch);
 return(ch);
}

// Log message with color based on log level
void log_message(LogLevel level, const char* message) {
    switch (level) {
        case LOG_DEBUG:
            printf("%s[DEBUG] %s%s\n", COLOR_CYAN, message, COLOR_RESET);
            break;
        case LOG_INFO:
            printf("%s[INFO] %s%s\n", COLOR_GREEN, message, COLOR_RESET);
            break;
        case LOG_WARNING:
            printf("%s[WARNING] %s%s\n", COLOR_YELLOW, message, COLOR_RESET);
            break;
        case LOG_ERROR:
            printf("%s[ERROR] %s%s\n", COLOR_RED, message, COLOR_RESET);
            break;
        default:
            break;
    }
}

void log_test(void)
{
    log_message(LOG_INFO, "Starting program...");
    log_message(LOG_DEBUG, "Debugging information...");
    log_message(LOG_WARNING, "Warning: Something might be wrong...");
    log_message(LOG_ERROR, "Error: Something went wrong!");
}