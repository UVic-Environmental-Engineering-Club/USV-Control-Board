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
#include "config.h"

//For testing on breadboard
#define blue_led        GPIO_NUM_15
#define green_led       GPIO_NUM_2

//GPS Related
#define PIN_SDA 21
#define PIN_CLK 22
#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

//static const char *TAG = "GPS: ";
nmea_parser_handle_t nmea_hdl;

//static char tag[] = "Compass: ";




void accelerometer_run(void);
void GPSInit(void);
void gps_handler_call(nmea_parser_handle_t);
void GPS_run(void);
void lidar_run(void);
void compass_run(void);


#endif