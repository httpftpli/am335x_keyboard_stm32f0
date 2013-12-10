#ifndef __TIMER__H__
#define __TIMER__H__

#include "stm32f0xx_conf.h"

extern volatile unsigned int timerTick1ms;

extern void timerInit(TIM_TypeDef* TIMx,unsigned short perscaler,unsigned short durationVal, unsigned short TIM_CounterMode,unsigned char isOnePulse, unsigned short intSource);
extern void timerTick1msInit(void);
extern void delayUs(unsigned int delayUs);
extern void delayMs(unsigned int ms);

  
#endif
 