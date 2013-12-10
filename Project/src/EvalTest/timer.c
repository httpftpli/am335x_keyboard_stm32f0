
#include "stm32f0xx_conf.h"
#include "stddef.h"



//TIMER 1 for delay us
//TIMER 3 for 1 ms timetick
//TIMER14 for beep during time
//TIMER16 for touchpad delay



volatile unsigned int timerTick1ms = 0;


void timerInit(TIM_TypeDef* TIMx,unsigned short perscaler,unsigned short durationVal, unsigned short TIM_CounterMode,unsigned char isOnePulse, unsigned short intSource){
  unsigned int RCC_APBPeriph_TIMx;
  unsigned int temp = (unsigned int)TIMx;
  switch(temp){
  case (unsigned int)TIM2:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM2;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case (unsigned int)TIM3:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM3;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case (unsigned int)TIM6:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM6;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case (unsigned int)TIM14:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM14; 
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM1:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM1;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM15:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM15;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM16:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM16;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM17:       
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM17;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;   
  default:
    return;
  }
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = perscaler-1;
  TIM_TimeBaseStructure.TIM_Period = durationVal;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode;
  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
  TIM_SelectOnePulseMode(TIMx, (isOnePulse==1)?TIM_OPMode_Single :TIM_OPMode_Repetitive);
  if(intSource!=0){
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
  }
}



void timerTick1msInit(void){ //timer for 1 ms interrupt 
  /*NVIC_InitTypeDef initstruct;
  initstruct.NVIC_IRQChannel = TIM14_IRQn;
  initstruct.NVIC_IRQChannelPriority = 0;
  initstruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&initstruct);
  timerInit(TIM14,1,SystemCoreClock/1000,TIM_CounterMode_Down,0, TIM_IT_Update);
  TIM_Cmd(TIM14, ENABLE);*/
  SysTick_Config(SystemCoreClock/1000);
}



void TIM100_IRQHandler(void){ //for 1 ms interrupt
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
  {
      TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
      TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
      TIM_ClearFlag(TIM14, TIM_IT_Update);
      timerTick1ms++;
  }
}


void delayUs(unsigned int delayUs){  //TIME1 for delay us
  timerInit(TIM1,SystemCoreClock/1000/1000,delayUs,TIM_CounterMode_Down,1, 0);
  while(TIM_GetITStatus(TIM1, TIM_IT_Update)!=SET)
  return;
}


void delayMs(unsigned int ms){
  unsigned int timetick = timerTick1ms;
  while((timerTick1ms-timetick)<= ms );
  return;
}

  
 
 