#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32F0xx.h"
#include "core_cm0.h"


/* Exported constants --------------------------------------------------------*/
extern const uint8_t TX_PACKET_1[24];


/* Exported macros -----------------------------------------------------------*/
#define  PLL_MUL_X  3
#define TX_PACKET_1_SIZE (sizeof(TX_PACKET_1))

/* typedef -------------------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;


#endif
