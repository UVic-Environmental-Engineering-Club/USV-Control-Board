#include "main.h"


void app_main() 
{
i2c_init();
rudderInit();
UARTInit();
RTOSInit();
}