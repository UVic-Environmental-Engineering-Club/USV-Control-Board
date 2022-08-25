#ifndef SENSORS
#define SENSORS


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
#include "i2c.h"
#include "sensors.h"

//GPS Related
#define PIN_SDA 21
#define PIN_CLK 22
#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

void accelerometer_run(void);
void GPSInit(void);
void gps_handler_call(nmea_parser_handle_t);
void GPS_run(void);
void lidar_run();
void compass_run();

#endif