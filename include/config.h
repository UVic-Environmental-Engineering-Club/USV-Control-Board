#ifndef CONFIG
#define CONFIG
#include "driver/uart.h"

//MAGNETOMETER Related
#define MAG_ID                      1
#define MAG_ADDR                    0x1C
#define MAG_REG_SIZE                1
#define MAG_WHO_AM_I_REG            0x0F
#define MAG_CTRL_REG1               0x20
#define MAG_CTRL_REG2               0x21
#define MAG_CTRL_REG3               0x22    
#define MAG_CTRL_REG4               0x23
#define MAG_CTRL_REG5               0x24
#define MAG_STATUS_REG              0x27 
#define MAG_OUT_X_L                 0x28
#define MAG_OUT_X_H                 0x29
#define MAG_OUT_Y_L                 0x2A
#define MAG_OUT_Y_H                 0x2B
#define MAG_OUT_Z_L                 0x2C
#define MAG_OUT_Z_H                 0x2D           
#define MAG_NUMREG                  1

void GPS_Init();
void UARTInit(void);
void compass_config();
void lidar_config();
void rudderInit(void);

#endif