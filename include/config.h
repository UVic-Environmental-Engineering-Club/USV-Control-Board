#ifndef CONFIG
#define CONFIG

#include "i2c.h"

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
#define PI 3.14159

sensor_t mag = {MAG_ID, MAG_ADDR, MAG_NUMREG};
byte mag_who_am_i = 0;
byte status_reg = 0;
byte magx1 = 0;
byte magx2 = 0;
byte magy1 = 0;
byte magy2 = 0;
byte magz1 = 0;
byte magz2 = 0;

//Lidar Related
sensor_t LIDAR1 = {1,0x62,23};
sensor_t LIDAR2 = {1,0x64,23};
sensor_t LIDAR3 = {1,0x66,23};
int lidar1_dist = 0;
int lidar2_dist = 0;
int lidar3_dist = 0;

void compass_config(void);
void lidar_config(void);
#endif