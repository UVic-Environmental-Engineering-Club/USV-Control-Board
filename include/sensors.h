#ifndef SENSORS
#define SENSORS

#include "rtos.h"
#include <stdio.h>
#include <stdlib.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nmea_parser.h"
#include <driver/i2c.h>
#include <math.h>
#include "sdkconfig.h"

void accelerometer_run(void);
void GPS_run(void);
void compass_run(void);

#endif