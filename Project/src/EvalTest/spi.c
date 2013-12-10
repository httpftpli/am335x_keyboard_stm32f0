#include "stm32f0xx_conf.h"

#define ADS7843_CS_GPIO             GPIOA
#define ADS7843_CS_GPIOBIT          GPIO_Pin_4
#define ADS7843_PORT            GPIOA 
#define ADS7843_SLK_PIN          GPIO_Pin_5
#define ADS7843_MISO_PIN         GPIO_Pin_6
#define ADS7843_MOSI_PIN         GPIO_Pin_7



void SpiCsEnable(void){
   GPIO_ResetBits(ADS7843_CS_GPIO,ADS7843_CS_GPIOBIT);
}

void SpiCsDisable(void){
   GPIO_SetBits(ADS7843_CS_GPIO,ADS7843_CS_GPIOBIT);
}


void spiInit(unsigned char CPOL,unsigned char CPHA ){ 
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = ADS7843_SLK_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(ADS7843_PORT, &GPIO_InitStruct);//时钟gpio 端口模式
  GPIO_InitStruct.GPIO_Pin = ADS7843_MISO_PIN;
  GPIO_Init(ADS7843_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin =ADS7843_MOSI_PIN;
  GPIO_Init(ADS7843_PORT, &GPIO_InitStruct);
  //CS
  GPIO_InitStruct.GPIO_Pin =ADS7843_CS_GPIOBIT;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_Init(ADS7843_CS_GPIO, &GPIO_InitStruct);
  //AF
  GPIO_PinAFConfig(ADS7843_PORT, ADS7843_SLK_PIN, GPIO_AF_1);
  GPIO_PinAFConfig(ADS7843_PORT, ADS7843_MISO_PIN, GPIO_AF_1);
  GPIO_PinAFConfig(ADS7843_PORT, ADS7843_MOSI_PIN, GPIO_AF_1);
  
  SpiCsDisable();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  SPI_InitTypeDef SPI_InitStruct;
  SPI_StructInit(&SPI_InitStruct);
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//配置spi 方向
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//配置spi 模式
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//配置数据格式
  
  SPI_InitStruct.SPI_CPOL = CPOL?SPI_CPOL_High:SPI_CPOL_Low;//配置时钟高电平稳态
  SPI_InitStruct.SPI_CPHA = CPHA?SPI_CPHA_2Edge:SPI_CPHA_1Edge;//配置时钟bit 位捕获方式
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//设置nss 管脚软件管理
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//设置spi 波特率分频

  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//指定数据传输从msb 位开始
  SPI_InitStruct.SPI_CRCPolynomial = 7;//指定用于CRC 计算的值
  SPI_Init(SPI1, &SPI_InitStruct);//调入结构体
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);//设置接收缓冲
  SPI_Cmd(SPI1, ENABLE); /*!< SD_SPI enable */
}


void spiSend(void *buf, uint32_t Length )
{
  uint32_t i;
  volatile uint8_t Dummy;
  unsigned char *buftemp = (unsigned char *)buf;
    
  for ( i = 0; i < Length; i++ )
  {		
	  /* Move on only TX FIFO not full. */
	  while (SPI_TransmissionFIFOStatus_Full==SPI_GetTransmissionFIFOStatus(SPI1));
	  SPI_SendData8(SPI1, buftemp[i]);
  }
  while ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)==SET);
}


void spiReceive(void *buf, uint32_t length )
{ 
  volatile unsigned char dumy ;
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==SET){
    dumy = SPI1->DR;
  }
  for(int i = 0;i<length;i++){
     SPI_SendData8(SPI1,0);
     while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
     ((unsigned char *)buf)[i] = SPI_ReceiveData8(SPI1);
  }
}


