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

int _write(int file, char *ptr, int len);

typedef struct
{
    log_level_t current_log_level;
    int (*_write)(char *ptr, int len);
}log_init_t;

static log_init_t led_config = {
    .current_log_level = LOG_LEVEL_TRACE, 
    ._write = NULL
};

// Called from syscalls in printf implementation,
// it will override weak implementation
#if OVERWRITE_PRINTF_WRITE
int _write(int file, char *ptr, int len)
{
    return led_config._write(ptr, len);
}
#endif

uint8_t log_set_level(log_level_t level)
{
    if (IS_VALID_LOG_LEVEL(level)) {
        led_config.current_log_level = level;
        return 0;
    }
    return 1; 
}

log_level_t log_get_level(void)
{
    return led_config.current_log_level;
}

uint8_t log_init(log_level_t log_level, int (*_write)(char *ptr, int len))
{
    led_config.current_log_level = log_level;
    led_config._write = _write;
    return 0;
};

void log_demo(void)
{
    const char *tag = "DEMO";
    log_message_tag(LOG_LEVEL_ERROR, tag, "Error: Something went wrong!\r\n");
    log_message_tag(LOG_LEVEL_WARNING, tag, "Warning: Something might be wrong...\r\n");
    log_message_tag(LOG_LEVEL_INFO, tag, "Starting program...\r\n");
    log_message_tag(LOG_LEVEL_DEBUG, tag, "Debugging information...\r\n");
    log_message_tag(LOG_LEVEL_TRACE, tag, "Trace: acquiring some data!\r\n");
}
