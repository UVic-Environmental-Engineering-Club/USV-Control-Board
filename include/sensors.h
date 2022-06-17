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
//GPS Related
#define PIN_SDA 21
#define PIN_CLK 22
#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

void accelerometer_run(void);
void GPSInit(void);
void gps_handler_call(nmea_parser_handle_t);
void GPS_run(void);
void compass_config(sensor_t);
void lidar_config(sensor_t, sensor_t, sensor_t);
void lidar_run(sensor_t, sensor_t, sensor_t);
void compass_run(sensor_t);

nmea_parser_handle_t nmea_hdl;

static const char *TAG = "GPS: ";













#endif