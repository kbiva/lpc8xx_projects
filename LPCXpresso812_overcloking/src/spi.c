/*
 *  spi.c
 *
 *  Author: Kestutis Bivainis
 *
 */
#include "chip.h"

/* Send/receive data via the SPI bus (assumes 9bit data) */
void spi0Transfer(uint16_t data) {

  while((LPC_SPI0->STAT & SPI_STAT_TXRDY) == 0);

  // 9bits,end of transfer,transmit slave select,receive ignore
  LPC_SPI0->TXDATCTL = SPI_TXCTL_FLEN(9-1) |
                       SPI_TXDATCTL_EOT |
                       //SPI_TXCTL_DEASSERT_SSEL |
	                     SPI_TXDATCTL_ASSERT_SSEL |
                       SPI_TXDATCTL_RXIGNORE |
                       data;
}
