#include "main.h"


void app_main() 
{
i2c_init();

GPSInit();

lidar_init();

RTOSInit();

}