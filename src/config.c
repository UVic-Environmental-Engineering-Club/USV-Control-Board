#include "config.h"
extern sensor_t mag;
extern byte mag_who_am_i;
extern byte status_reg;
extern byte magx1;
extern byte magx2;
extern byte magy1;
extern byte magy2;
extern byte magz1;
extern byte magz2;

extern sensor_t LIDAR1;
extern sensor_t LIDAR2;
extern sensor_t LIDAR3;
extern int lidar1_dist;
extern int lidar2_dist;
extern int lidar3_dist;

//Compass Config
void compass_config(void)
{
    i2c_yeet(mag, MAG_CTRL_REG2, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG1, 0x54, MAG_REG_SIZE); //Reg config
    i2c_yeet(mag, MAG_CTRL_REG3, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG4, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG5, 0x00, MAG_REG_SIZE);
    i2c_yoink(mag, MAG_WHO_AM_I_REG, &mag_who_am_i, MAG_REG_SIZE);
}
//Lidar Config
void lidar_config(void)
{
    i2c_yeet(LIDAR1,0x18,(byte)8,1);                  //switches lidar2's address from default to 0x64 by reading it's 16 bit serial number into registers 0x18 & 0x19 etc.
    i2c_yeet(LIDAR1,0x19,(byte)38,1);
    i2c_yeet(LIDAR1,0x1a,(LIDAR2.addr<<1),1);
    i2c_yeet(LIDAR1,0x1e,0x10,1);
    i2c_yeet(LIDAR2,0x1e,0x18,1);
	i2c_yeet(LIDAR1,0x18,(byte)8,1);                  //switches lidar3's address from default to 0x66 by reading it's 16 bit serial number into registers 0x18 & 0x19 etc.
    i2c_yeet(LIDAR1,0x19,(byte)34,1);
    i2c_yeet(LIDAR1,0x1a,(LIDAR3.addr<<1),1);
    i2c_yeet(LIDAR1,0x1e,0x10,1);
    i2c_yeet(LIDAR2,0x1e,0x18,1);
}