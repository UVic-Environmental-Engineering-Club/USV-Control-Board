#ifndef COMMS
#define COMMS

#include "rtos.h"

extern int accelx,accely,accelz;
extern int gyrx,gyry,gyrz;
extern int lid1,lid2,lid3;
extern int heading;
extern int rpm1,rpm2;
extern int tmp1,tmp2;
extern int wet1,wet2;
extern int longitude,latitude;
extern int mpr;
extern int ran;

void UART_run(void);
void I2C_run(void);

#endif