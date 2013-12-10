#include "stm32f0xx_conf.h"
#include "lcd_der.h"


#define DS2412_ADDR   0x76




unsigned char DS2412Write(unsigned char addr,unsigned char data){ 
  I2C_TransferHandling(I2C1, DS2412_ADDR<<1, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  while(1){
    if(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS)) break;
    if(I2C_GetFlagStatus(I2C1, I2C_ISR_NACKF)) return 0;
  }
  I2C_SendData(I2C1, addr);
  while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET);
  I2C_SendData(I2C1, data);
  return 1;
}


unsigned char DS2412Read(unsigned char addr,unsigned char *data){
  I2C_TransferHandling(I2C1, DS2412_ADDR<<1, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  while(1){
    if(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS)) 
      break;
    if(I2C_GetFlagStatus(I2C1, I2C_ISR_NACKF)) 
     return 0;
  }
  I2C_SendData(I2C1, addr);
  while(I2C_GetFlagStatus(I2C1, I2C_ISR_TC) == RESET);
  I2C_TransferHandling(I2C1, DS2412_ADDR<<1, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
  *data = I2C_ReceiveData(I2C1);
  return 1;
}


unsigned char DS2412ReadSlaveId(){
  unsigned char id;
  if(DS2412Read(DS2412_REG_SLAVE_ID,&id)==1)
    return id;
  return 0;
}



unsigned char DS2412SetEq(unsigned char EQGain){
  unsigned char data;
  DS2412Read(DS2412_REG_FEATURES2,&data);
  data &= ~(0x03<<5);
  data |= (EQGain<<5)|(1<<4);// EQ ENABLE
  DS2412Write(DS2412_REG_FEATURES2,data);
  return 1;
}




