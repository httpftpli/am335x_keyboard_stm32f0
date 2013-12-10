#include "stm32f0xx_conf.h"
#include "timer.h"



#define BEEPIN_GPIO      GPIOA
#define BEEPIN_GPIOBIT   GPIO_Pin_0

#define BEEPOUT_GPIO     GPIOA
#define BEEPOUT_GPIOBIT  GPIO_Pin_1
#define BEEP_ON  Bit_SET
#define BEEP_OFF  Bit_RESET

volatile int keybeepin = 0;


void TIM3_IRQHandler(void){ //timer 3 for beep during time
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
      
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
      TIM_ClearFlag(TIM3, TIM_IT_Update);
      GPIO_WriteBit(BEEPOUT_GPIO, BEEPOUT_GPIOBIT,Bit_RESET); //turn off beep
      keybeepin = 0;
  }
}



void beepInit(void){
  //beep out
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = BEEPOUT_GPIOBIT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(BEEPOUT_GPIO, &GPIO_InitStructure); 
  //beep in
  GPIO_InitStructure.GPIO_Pin = BEEPIN_GPIOBIT;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(BEEPIN_GPIO, &GPIO_InitStructure);
  
  NVIC_InitTypeDef initstruct;
  initstruct.NVIC_IRQChannel = TIM3_IRQn;
  initstruct.NVIC_IRQChannelPriority = 0;
  initstruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&initstruct);
  timerInit(TIM3,SystemCoreClock/1000,70,TIM_CounterMode_Down,1, TIM_IT_Update);
}



void beepProcess(void){
     if(!GPIO_ReadInputDataBit(BEEPIN_GPIO, BEEPIN_GPIOBIT)){
         GPIO_WriteBit( BEEPOUT_GPIO, BEEPOUT_GPIOBIT,BEEP_ON);
     }else if(keybeepin==0){
         GPIO_WriteBit( BEEPOUT_GPIO, BEEPOUT_GPIOBIT,BEEP_OFF);
     }
}

void keyBeep(void){
   GPIO_WriteBit(BEEPOUT_GPIO, BEEPOUT_GPIOBIT,BEEP_ON);
   keybeepin = 1;
   TIM_Cmd(TIM3, ENABLE);
}


