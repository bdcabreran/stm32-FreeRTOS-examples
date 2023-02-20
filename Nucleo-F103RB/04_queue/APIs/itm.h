#ifndef ITM_H
#define ITM_H

#include "stm32f1xx.h"

uint8_t itm_send_char(uint8_t ch, uint8_t port);
int itm_write_ch0(char *ptr, int len);


#endif