#ifndef RTOS
#define RTOS

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "communications.h"
#include "motorcontrol.h"
#include "sensors.h"
#include "nmea_parser.h"

#define UART_TASK_RUN_PERIOD 50
#define I2C_TASK_RUN_PERIOD 50
#define MOTOR_TASK_RUN_PERIOD 50
#define GPS_TASK_RUN_PERIOD 10000
#define ACCEL_TASK_RUN_PERIOD 50
#define COMPASS_TASK_RUN_PERIOD 50

extern xTaskHandle UART_task;
extern xTaskHandle I2C_task;
extern xTaskHandle motorControl_task;
extern xTaskHandle GPS_task;
extern xTaskHandle accelerometer_task;
extern xTaskHandle compass_task; 

extern nmea_parser_handle_t nmea_hdl;

void RTOSInit(void);


/**
 * CORE 1 TASKS
 */
void UART_PRIVATETASK(void* params);
void I2C_PRIVATETASK(void* params);

/**
 * CORE 0 TASKS
 */
void motorControl_PRIVATETASK(void* params);
void GPS_PRIVATETASK(void* params);
void accelerometer_PRIVATETASK(void* params);
void compass_PRIVATETASK(void* params);


#endif