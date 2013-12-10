#include "stm32f0xx_conf.h"

void UARTInit(USART_TypeDef* USARTx, unsigned int bondrate){
  unsigned int temp = (unsigned int) USARTx;
  switch (temp){
  case (unsigned int)USART1:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    break;
  case (unsigned int)USART2:
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    break;
  default:
    break;
  }
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//使能GPIO 时钟
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);//复用功为1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //设置端口复用
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = bondrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  //USART_DMACmd(USARTx, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
  USART_Cmd(USARTx, ENABLE);   
}


void UARTSend(USART_TypeDef * USARTx ,void  *Buffer, uint32_t Length){
  unsigned char * buf = Buffer;
  while(Length != 0){
  while(!((USARTx->ISR)&(1<<7)));//等待发送完
     USARTx->TDR= *buf;
     buf++;
     Length--;
  }
}

