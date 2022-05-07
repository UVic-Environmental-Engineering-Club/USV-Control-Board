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
#include "i2c.h"

#define PIN_SDA 21
#define PIN_CLK 22
#define I2C_ADDRESS 0x1e
#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);
static char tag[] = "Compass: ";
static const char *TAG = "GPS: ";

nmea_parser_handle_t nmea_hdl;

sensor_t LIDAR1 = {1,0x62,23};
sensor_t LIDAR2 = {1,0x64,23};
int lidar1_dist;
int lidar2_dist;

void GPSInit(void);

void accelerometer_run(void);
void GPS_run();
void compass_run(i2c_cmd_handle_t);
i2c_cmd_handle_t compass_config_run(void);
void gps_handler_call(nmea_parser_handle_t);
void lidar_init();
void lidar_run();

#endif