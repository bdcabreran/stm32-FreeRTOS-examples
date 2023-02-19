/*
 * rtt.c
 *
 *  Created on: Feb 18, 2023
 *      Author: BAYRON
 */

#include "rtt.h"
#include "SEGGER_RTT.h"
#include <stdint.h>
#include <assert.h>

#define RTT_TX_BUFF_SIZE  (512)
#define RTT_RX_BUFF_SIZE  (512)

static uint8_t tx_buffer[RTT_TX_BUFF_SIZE];
static uint8_t rx_buffer[RTT_RX_BUFF_SIZE];

typedef struct
{
    const char *name;
    uint8_t  channel;
    struct {
        uint8_t *buffer;
        size_t   len;
    }tx;

    struct {
        uint8_t *buffer;
        size_t   len;
    }rx;
}rtt_config_t;


static rtt_config_t rtt_config = 
{
    .channel = 0, 
    .name = "channel 0", 
    .tx.buffer = tx_buffer, 
    .tx.len = RTT_TX_BUFF_SIZE,
    .rx.buffer = rx_buffer, 
    .rx.len = RTT_RX_BUFF_SIZE
};

// SEGGER RTT library supports 16 channels, the buffer index values will range from 0 to 15.
#define IS_VALID_CHANNEL(ch) (ch >= 0 && ch <= 15 ) 

// Init RTT default configuration 
void rtt_init(void)
{
    assert(IS_VALID_CHANNEL(rtt_config.channel));

    // Initialize RTT
	SEGGER_RTT_Init();

    // Configure RTT channel (Target - PC)
    if(rtt_config.tx.len) {
        SEGGER_RTT_ConfigUpBuffer(rtt_config.channel, rtt_config.name,
                                rtt_config.tx.buffer, rtt_config.tx.len,
                                SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    }

    // Configure RTT channel (PC - Target)
    if(rtt_config.rx.len) {
        SEGGER_RTT_ConfigDownBuffer(rtt_config.channel, rtt_config.name,
                                rtt_config.rx.buffer, rtt_config.rx.len,
                                SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    }
}

int rtt_write(char *ptr, int len)
{
  SEGGER_RTT_Write(rtt_config.channel, ptr, len);
  return len;
}

int rtt_get_data_len(void)
{
    return SEGGER_RTT_HasData(rtt_config.channel);
}

int rtt_read(char *ptr, int len)
{
    return SEGGER_RTT_Read(rtt_config.channel, ptr, len);
}
