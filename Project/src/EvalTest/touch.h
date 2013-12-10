#ifndef __TOUCH__H___
#define __TOUCH__H___


extern volatile unsigned int touched;

extern unsigned short touchpointX ,touchpointY;

extern void touchInit();
extern void touchSampleExec();

#endif

    