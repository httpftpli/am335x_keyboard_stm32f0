
#include "stm32f0xx_conf.h"
#include "timer.h"
#include "beep.h"
#include "uart.h"
#include "blacklight.h"
#include "key.h"
#include "touch.h"
#include "i2c.h"
#include "lcd_der.h"





#define MSG_TYPE_KEY    1
#define MSG_TYPE_TOUCH  2
#define MSG_TYPE_KEYRESET 4


#pragma pack(1)
typedef struct __message{
  unsigned char type;
  unsigned char magic;
  unsigned short keycode;
  unsigned int tscval;
} MSG;
#pragma pack()

MSG msg = {.magic = 0x55};

unsigned int ctrlbeep=0;


volatile unsigned short xsam,ysam;


void GPIOInit(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC| RCC_AHBPeriph_GPIOF, ENABLE);
}


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
  int send = 0;
  SystemCoreClockUpdate ();   
  timerTick1msInit();
  GPIOInit();
  beepInit();
  UARTInit(USART1,57600);
  blPinInit(); 
  keyScanInit();
  touchInit(); 
  i2cInit();
    
  unsigned char data;
  //DS2412SetEq(EQGAIN_M1P625);
  //DS2412Read(DS2412_REG_FEATURES2,&data);
 
#ifdef NDEBUG
   WDTInit(125000);
#endif
   unsigned int keycode =-1;
   while(1){
#ifdef NDEBUG
      WDTFeed();
#endif
      keycode = keyScan();
      if(keycode!=-1){
        msg.type |= MSG_TYPE_KEY;
        msg.keycode = (unsigned short)keycode;
        keyBeep();
        send = 1;
      } 
      //beepProcess();      
      blProcess();
      touchSampleExec();
      ////////////////////////////  
#if 0
      static unsigned int timerbuf[256];
      static int timerbufindex = 0;
      static unsigned int oldtimer ;
#endif
      ////////////////////////////
        if(touched==1){
       ////////////////////////////    
        #if 0
        timerbuf[timerbufindex++] = timerTick1ms-oldtimer ;
        oldtimer = timerTick1ms;
        if(timerbufindex == 256){
          timerbufindex =0 ;
        }
        #endif
        ////////////////////////////          
        touched=0; 
        msg.type |= MSG_TYPE_TOUCH;
        msg.tscval = touchpointY<<16 | touchpointX;
        send = 1;
      }
      if(1==send){
        UARTSend(USART1,&msg,sizeof msg);
        send =0;
        msg.type = 0;
      }   
  }
}
  

 
  
 
