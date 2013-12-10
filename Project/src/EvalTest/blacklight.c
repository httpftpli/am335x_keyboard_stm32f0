#include "stm32f0xx_conf.h"

#define LCDBLIN_GPIO     GPIOA
#define LCDBLIN_GPIOBIT  GPIO_Pin_2 

#define LCDBL_GPIO     GPIOA
#define LCDBL_GPIOBIT  GPIO_Pin_11

#define LCDBLOUT_ON  Bit_SET
#define LCDBLOUT_OFF  Bit_RESET


void blPinInit(void){
  //backlight out
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = LCDBL_GPIOBIT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(LCDBL_GPIO, &GPIO_InitStructure); 
  //backlight in
  GPIO_InitStructure.GPIO_Pin = LCDBLIN_GPIOBIT;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(LCDBLIN_GPIO, &GPIO_InitStructure);
 
}


void blProcess(void){
  if(!GPIO_ReadInputDataBit(LCDBLIN_GPIO,LCDBLIN_GPIOBIT)){
        GPIO_WriteBit(LCDBL_GPIO, LCDBL_GPIOBIT, LCDBLOUT_ON); //LCD BLACKLIGTH ON 
  }else{
        GPIO_WriteBit(LCDBL_GPIO, LCDBL_GPIOBIT, LCDBLOUT_OFF); //LCD BLACKLIGTH OFF
  }
}