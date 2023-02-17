#include "itm.h"

uint8_t itm_send_char(uint8_t ch, uint8_t channel)
{
  uint32_t port = 0;

  // Check if ITM is enabled and the channel is enabled
  if ((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0 &&
      (ITM->TER & (1 << channel)) != 0)
  {
      // Get the stimulus port number for the channel
      port = ITM->TPR & ~(0x3);
      port = channel;

      // Wait until the stimulus port is ready to accept data
      while (ITM->PORT[port].u32 == 0){ __NOP();}

      // Write the character to the stimulus port for the channel
      ITM->PORT[port].u8 = (uint8_t)ch;
  }

  return ch;
}

int itm_write_ch0(char *ptr, int len)
{
  // Write character to ITM ch.0
  for (int DataIdx = 0; DataIdx < len; DataIdx++) {
      itm_send_char(*ptr++, 0);
  }
  return len;
}
