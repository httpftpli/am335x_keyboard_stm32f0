#ifndef __SPI__H__
#define __SPI__H__

extern void SpiCsEnable(void);
extern void SpiCsDisable(void);
extern void spiInit(unsigned char CPOL,unsigned char CPHA );
extern void spiSend(void *buf, uint32_t Length );
extern void spiReceive(void *buf, uint32_t length );

#endif