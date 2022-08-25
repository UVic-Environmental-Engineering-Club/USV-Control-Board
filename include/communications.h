#ifndef COMMS
#define COMMS
#include <rtos.h>
// #include "driver/uart.h"
// #include <stdio.h>
// #include <nmea_parser.h>
// #include <sensors.h>

void UART_run(void);
void I2C_run(void);

extern int ran;
extern int mpr;

#endif