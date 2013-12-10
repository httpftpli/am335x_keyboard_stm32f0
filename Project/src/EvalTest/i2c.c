#include "stm32f0xx_conf.h"


void i2cInit() 
{
  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);//使能GPIO 时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE); //使能i2c 时钟
  
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;   //配置端口复用
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOB , &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOB , &GPIO_InitStruct);
  GPIO_PinAFConfig( GPIOB , GPIO_PinSource8, GPIO_AF_1);
  GPIO_PinAFConfig( GPIOB ,GPIO_PinSource9, GPIO_AF_1); //设置复用通道
  
  I2C_InitTypeDef I2C_InitStruct;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;//配置模式
  I2C_InitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable;//使能模拟噪声滤波
  I2C_InitStruct.I2C_DigitalFilter = 0x01;
  I2C_InitStruct.I2C_OwnAddress1 =0x00;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;//使能应答
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStruct.I2C_Timing = 1<<28  |0xC7| 0xC3<<8 | 0x2<<16 | 0x4<<20; //10KHZ
  I2C_Cmd(I2C1, ENABLE);//使能i2c 
  I2C_Init(I2C1, &I2C_InitStruct);
}