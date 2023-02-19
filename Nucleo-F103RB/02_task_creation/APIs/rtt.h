/*
 * rtt.h
 *
 *  Created on: Feb 18, 2023
 *      Author: BAYRON
 */

#ifndef RTT_H
#define RTT_H

void rtt_init(void);
int rtt_write(char *ptr, int len);
int rtt_get_data_len(void);
int rtt_read(char *ptr, int len);

#endif


