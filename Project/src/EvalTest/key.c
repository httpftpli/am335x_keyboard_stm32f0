#include "stm32f0xx_conf.h"
#include "timer.h"

typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;


GPIOGROUP keyScanOutput[8] ={
  {GPIOA,GPIO_Pin_15},{GPIOB,GPIO_Pin_3},{GPIOB,GPIO_Pin_4},{GPIOB,GPIO_Pin_5},
  {GPIOB,GPIO_Pin_6},{GPIOB,GPIO_Pin_7},{GPIOB,GPIO_Pin_2},{GPIOB,GPIO_Pin_1}
};

GPIOGROUP keyScanInput[8] ={
  {GPIOA,GPIO_Pin_12},{GPIOA,GPIO_Pin_8},{GPIOB,GPIO_Pin_15},{GPIOB,GPIO_Pin_14},
  {GPIOB,GPIO_Pin_13},{GPIOB,GPIO_Pin_12},{GPIOF,GPIO_Pin_7},{GPIOF,GPIO_Pin_6}
};


static void leyScanOutputReset(void){
  for(int i=0;i<(sizeof(keyScanOutput)/sizeof(GPIOGROUP));i++){
    GPIO_SetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
  }
}

void keyScanInit(void){
   GPIO_InitTypeDef GPIO_InitStructure;  
   //output
   for(int i=0;i<(sizeof(keyScanOutput)/sizeof(GPIOGROUP));i++){    
      GPIO_InitStructure.GPIO_Pin = keyScanOutput[i].bit;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(keyScanOutput[i].port, &GPIO_InitStructure);           
   }
   //input
   for(int i=0;i<(sizeof(keyScanInput)/sizeof(GPIOGROUP));i++){
      GPIO_InitStructure.GPIO_Pin = keyScanInput[i].bit;	
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
      GPIO_Init(keyScanInput[i].port, &GPIO_InitStructure);
   }
   //output 1
   leyScanOutputReset();
}


void keyScanOutputTest(void){
  for(int i=0;i<(sizeof(keyScanOutput)/sizeof(GPIOGROUP));i++){
    GPIO_ResetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
  }
}
  



#define TIMEINTERVAL_2KEY        500 //500ms
#define TIMEINTERVAL_DEBOUNCE    10  //10ms
#define TIMEINTERVAL_KEYHOLD     100  //10ms

#define NOKEY                   0
#define KEYTODEBOUNCE           1
#define KEYPUSHED               2
#define KEYHOLD                 3


signed int keyScan(void){
  static unsigned short keycode;
  static unsigned int keystate = NOKEY;
  static unsigned int timermark;
  unsigned short keycodenow;
  for(int i=0;i<(sizeof(keyScanOutput)/sizeof(GPIOGROUP));i++){
    leyScanOutputReset();
    GPIO_ResetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
    delayUs(20);
    for(int j=0;j<(sizeof(keyScanInput)/sizeof(GPIOGROUP));j++){
        unsigned int bitval = GPIO_ReadInputDataBit(keyScanInput[j].port, keyScanInput[j].bit);
        if(0==bitval){
         keycodenow = (i<<8)|j;
         goto KEYPROCESS;
        }
    }
  }
  keystate = NOKEY;
  return -1;
  
KEYPROCESS:
    switch(keystate){
    case NOKEY:
      keystate = KEYTODEBOUNCE;
      keycode = keycodenow;
      timermark = timerTick1ms;
      break;
    case KEYTODEBOUNCE:
      if(keycodenow==keycode){
        if((timerTick1ms-timermark)>=TIMEINTERVAL_DEBOUNCE){
          keystate = KEYPUSHED;
          keycode = keycodenow;
          timermark = timerTick1ms;
          return keycode;
        }
      }else{
        keystate = NOKEY;
      }
      break; 
    case KEYPUSHED:
      if(keycodenow==keycode){
        if((timerTick1ms-timermark)>=TIMEINTERVAL_2KEY){
          keystate = KEYHOLD;
          keycode = keycodenow;
          timermark = timerTick1ms;
          return keycode;
        }
      }else {
        keystate = NOKEY;
      }
      break;
    case KEYHOLD:
      if(keycodenow==keycode){
        if((timerTick1ms-timermark)>=TIMEINTERVAL_KEYHOLD){
          keystate = KEYHOLD;
          keycode = keycodenow;
          timermark = timerTick1ms;
          return keycode;
        }
      }else {
        keystate = NOKEY;
      }
      break;
    default:
      break;
    }  

return -1;      
}  