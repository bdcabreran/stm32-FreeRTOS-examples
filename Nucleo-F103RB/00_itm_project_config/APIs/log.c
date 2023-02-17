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
#include "stdarg.h"

static log_level_t current_log_level = LOG_LEVEL_INFO;

int __io_putchar(int ch)
{
    // Write character to ITM ch.0
    ITM_SendChar(ch);
    return (ch);
}


uint8_t log_message_set_level(log_level_t level)
{
    if (IS_VALID_LOG_LEVEL(level))
    {
        current_log_level = level;
        return 0;
    }
    return 1; 
}

log_level_t log_message_get_level(void)
{
    return current_log_level;
}


#if 0
void log_message(log_level_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);

    if (level <= current_log_level) {
        
        const char *color = "";
        switch (level)
        {
        case LOG_LEVEL_ERROR:   color = COLOR_RED;     break;
        case LOG_LEVEL_WARNING: color = COLOR_YELLOW;  break;
        case LOG_LEVEL_INFO:    color = COLOR_GREEN;   break;
        case LOG_LEVEL_DEBUG:   color = COLOR_BLUE;    break;
        case LOG_LEVEL_TRACE:   color = COLOR_CYAN;    break;
        }

        printf("%s", color);
        vprintf(format, args);
        printf("%s", COLOR_RESET);
    }

    va_end(args);
}
#endif 

void log_test(void)
{
    log_message(LOG_LEVEL_ERROR, "Error: Something went wrong!\r\n");
    log_message(LOG_LEVEL_WARNING, "Warning: Something might be wrong...\r\n");
    log_message(LOG_LEVEL_INFO, "Starting program...\r\n");
    log_message(LOG_LEVEL_DEBUG, "Debugging information...\r\n");
    log_message(LOG_LEVEL_TRACE, "Trace: acquiring some data!\r\n");
}
