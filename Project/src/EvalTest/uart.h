#ifndef __UART__H__
#define __UART__H__

#include "stm32f0xx_conf.h"

extern void UARTInit(USART_TypeDef* USARTx, unsigned int bondrate);

extern void UARTSend(USART_TypeDef * USARTx ,void  *Buffer, uint32_t Length);

#endif