#include "stm32f0xx_conf.h"
#include "spi.h"
#include "timer.h"

#define ADS7843_INT_GPIO            GPIOA
#define ADS7843_INT_GPIOBIT         GPIO_Pin_3

#define ADS7843_CTRL_X_SAMPLE   0xd0   //0b11010000
#define ADS7843_CTRL_Y_SAMPLE   0x90   //0b10010000

#define NARRAY(array) (sizeof((array))/sizeof((array[0])))



int debugbufx[500], debugbufy[500];
int debugbufindex = 0,debugbufindex1 = 0;
int npoint = 0;

volatile unsigned int touched = 0;

unsigned short touchpointX ,touchpointY;

void bubbleSortAscend(int *buf, unsigned int nNum) {
   int j, k;
   int flag = nNum;
   unsigned int tmp;
   while (flag > 0) {
      k = flag;
      flag = 0;
      for (j = 1; j < k; j++){
         if (buf[j - 1] > buf[j]) {
            tmp = buf[j-1];
            buf[j - 1] = buf[j];
            buf[j] = tmp;
            flag = j;
         }
      }
   }
}

int sum(int *buf,unsigned int nNum){
   unsigned int sum = 0;
   for (int i=0;i<nNum;i++) {
      sum += buf[i];
   }
   return sum;
}


static void  touchTimerInit();

void touchInit(){
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = ADS7843_INT_GPIOBIT;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(ADS7843_INT_GPIO, &GPIO_InitStruct); 
  spiInit(0,0);
  touchTimerInit();
}



unsigned short touchXsample(){
  unsigned char rcvbuf[2];
  unsigned char sendval = ADS7843_CTRL_X_SAMPLE;
  SpiCsEnable();
  spiSend(&sendval, 1); 
  spiReceive(rcvbuf, sizeof rcvbuf);
  SpiCsDisable();
  unsigned short val = (((unsigned short)rcvbuf[0])&0x7f)<<5;
  return val | (rcvbuf[1])>>3;
}


unsigned short touchYsample(){
  unsigned char rcvbuf[2];
  unsigned char sendval = ADS7843_CTRL_Y_SAMPLE;
  SpiCsEnable();
  spiSend(&sendval, 1);
  spiReceive(rcvbuf, sizeof rcvbuf );
  SpiCsDisable();
  unsigned short val = (((unsigned short)rcvbuf[0])&0x7f)<<5;
  return val | (rcvbuf[1])>>3;
}


void  touchSample(int *x, int *y){
     int xsampleval[5],ysampleval[5];
     //GPIOSetValue(3, 0, 1);  //to debug
     for(int i=0;i<5;i++){
        xsampleval[i] = touchXsample();
     }
     bubbleSortAscend(xsampleval,NARRAY(xsampleval));
     for(int i=0;i<5;i++){
        ysampleval[i] = touchYsample();
     }
     bubbleSortAscend(ysampleval,NARRAY(ysampleval));    
     //GPIOSetValue(3, 0, 0); //to debug
     *y =  sum(ysampleval+1,NARRAY(ysampleval)-2)/(NARRAY(ysampleval)-2);
     *x =  sum(xsampleval+1,NARRAY(xsampleval)-2)/(NARRAY(xsampleval)-2);
     //debugbufx[debugbufindex] = *x;
     //debugbufy[debugbufindex++] = *y;
     
}



static void touchTimerInit(){
  NVIC_InitTypeDef initstruct;
  initstruct.NVIC_IRQChannel = TIM14_IRQn;
  initstruct.NVIC_IRQChannelPriority = 0;
  initstruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&initstruct);
  timerInit(TIM14,(SystemCoreClock/1000/1000),0xffff, TIM_CounterMode_Up,0, TIM_IT_Update);
  TIM_Cmd(TIM14, ENABLE);
}


static volatile unsigned short touchtimertickhight = 0;
void TIM14_IRQHandler(void){
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
  {
      TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
      //TIM_ClearFlag(TIM14, TIM_IT_Update);
      touchtimertickhight++;  
  }
}



void touchTimerReset(){
  TIM14->CNT = 0;
  touchtimertickhight = 0;
}

unsigned int touchGetTime(){
  unsigned short temp = touchtimertickhight;
  return temp<<16+TIM14->CNT;
}


#if 0
void touchSampleExec(){
#define NO_TOUCH      0
#define TOUCHED_RELEASE  3
#define TOUCH_SAMPLE_BEGIN  5
#define TOUCH_DEBOUNCE    7
  static int samplecounter = 0,stat = NO_TOUCH;
  static unsigned int sumx = 0,sumy = 0;
  static unsigned short lastx = 0,lasty = 0;

  switch(stat){
    case NO_TOUCH:
      if(!GPIOGetBitValue(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
          touchSample(&lastx,&lasty);
          sumx += lastx;
          sumy += lasty;
          samplecounter++;
          stat = TOUCHE_SAMPLE_BEGIN;
          touchTimerReset();   //复位定时器
      }
      break;
  case TOUCHE_SAMPLE_BEGIN:
      if(!GPIOGetBitValue(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
          touchSample(&lastx,&lasty);
          sumx += lastx;
          sumy += lasty;
          samplecounter++;
          stat = TOUCHE_SAMPLE_BEGIN;
          if(touchGetTime()>50000){ //100ms
             touchpointX = (sumx)/(samplecounter);
             touchpointY = (sumy)/(samplecounter);
             sumx = 0;
             sumy = 0;
             samplecounter = 0;
             touched = 1;
             touchTimerReset();            
          }
      }else{
        {
        if(samplecounter>=5){
          touchpointX = (sumx-lastx)/(samplecounter-1);
          touchpointY = (sumy-lasty)/(samplecounter-1);          
          touched = 1;
        }else{
          npoint = samplecounter;
        }
        }
        sumx = 0;
        sumy = 0;
        samplecounter = 0;
        stat = NO_TOUCH; 
      }
  default:
      break;
  }
}
#endif

void touchSampleExec(){
#define NO_TOUCH            0
#define TOUCHED     3
#define TOUCH_SAMPLE_BEGIN  5
#define TOUCH_DEBOUNCE      7
#define TOUCH_FROZE       4
  
#define TOUCH_SAMPLE_NUM   7
  static int samplecounter = 0,stat = NO_TOUCH;  
  static int xbuf[TOUCH_SAMPLE_NUM+2],ybuf[TOUCH_SAMPLE_NUM+2];  
  
  switch(stat){
    case NO_TOUCH:
      if(!GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
         stat = TOUCH_DEBOUNCE;
         touchTimerReset();
      }
      break;
  case TOUCH_DEBOUNCE:
    if(GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      if(touchGetTime()>16000){
        samplecounter = 0;
        stat = TOUCH_SAMPLE_BEGIN;
      }
    }
    break;
  case TOUCH_SAMPLE_BEGIN:
     if(GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      touchSample(xbuf+samplecounter,ybuf+samplecounter);     
      samplecounter ++;
      if(samplecounter >= TOUCH_SAMPLE_NUM+1){
         samplecounter --;
         bubbleSortAscend(xbuf, samplecounter);
         bubbleSortAscend(ybuf, samplecounter);
         if(((xbuf[TOUCH_SAMPLE_NUM-2-1]-xbuf[2])<10)&&
            ((ybuf[TOUCH_SAMPLE_NUM-2-1]-ybuf[2])<10)){
           touchpointX = sum(xbuf+2,samplecounter-4)/(samplecounter-4);
           touchpointY = sum(ybuf+2,samplecounter-4)/(samplecounter-4);        
           touched = 1;
           stat = TOUCHED;
          }
         samplecounter = 0;                              
         touchTimerReset();        
      }     
    }
    break;
  case TOUCHED:
     if(GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      //touchTimerReset();
      touchSample(xbuf+samplecounter,ybuf+samplecounter);     
      samplecounter ++;
      if((samplecounter >= TOUCH_SAMPLE_NUM+1)){ 
          samplecounter --;
         bubbleSortAscend(xbuf, samplecounter);
         bubbleSortAscend(ybuf, samplecounter);
          if(((xbuf[TOUCH_SAMPLE_NUM-2-1]-xbuf[2])<10)&&
            ((ybuf[TOUCH_SAMPLE_NUM-2-1]-ybuf[2])<10)){
            touchpointX = sum(xbuf+2,samplecounter-4)/(samplecounter-4);
            touchpointY = sum(ybuf+2,samplecounter-4)/(samplecounter-4); 
            touched = 1;
            stat = TOUCH_FROZE;
         }
         samplecounter = 0;         
         //touchTimerReset();
      }
    }
    break;
  case TOUCH_FROZE:
    if(GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT)){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      if(touchGetTime()>50000){
        stat = TOUCHED;
        samplecounter = 0;
        touchTimerReset();
      }
    }
    break;
  default:
    break;
  }
}
    