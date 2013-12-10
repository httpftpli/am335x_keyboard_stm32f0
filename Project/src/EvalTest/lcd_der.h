#ifndef __LCD_DER__H__
#define __LCD_DER__H__


#define DS2412_REG_CONFIG1     0
#define DS2412_REG_SLAVE_ID    1
#define DS2412_REG_FEATURES1   2
#define DS2412_REG_FEATURES2   3
#define DS2412_REG_ROUT        4


#define EQGAIN_M1P625      0
#define EQGAIN_M3P25       1
#define EQGAIN_M4P87       2
#define EQGAIN_M6P5        3
#define EQGAIN_M8P125      4
#define EQGAIN_M9P75       5
#define EQGAIN_M11P375     6
#define EQGAIN_M13P        7


extern unsigned char DS2412Write(unsigned char addr,unsigned char data);
extern unsigned char DS2412ReadSlaveId(void);


#endif