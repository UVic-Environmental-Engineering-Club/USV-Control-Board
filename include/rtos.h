#ifndef RTOS
#define RTOS

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "nmea_parser.h"
#include "i2c.h"
#include "config.h"
#include "sensors.h"
#include "motorcontrol.h"
#include "communications.h"


#define UART_TASK_RUN_PERIOD 500
#define I2C_TASK_RUN_PERIOD 250
#define MOTOR_TASK_RUN_PERIOD 50

extern xTaskHandle UART_task;
extern xTaskHandle I2C_task;
extern xTaskHandle motorControl_task;

extern nmea_parser_handle_t nmea_hdl;
extern nmea_parser_config_t config;
extern sensor_t mag;
extern sensor_t LIDAR1;
extern sensor_t LIDAR2;
extern sensor_t LIDAR3;

extern byte status_reg;
extern byte magx1;
extern byte magx2;
extern byte magy1;
extern byte magy2;
extern byte magz1;
extern byte magz2;

extern byte lidar_val1;
extern byte lidar_val2;

extern int accelx,accely,accelz;
extern int gyrx,gyry,gyrz;
extern int lid1,lid2,lid3;
extern int compass_x,compass_y,compass_z;
extern int rpm1,rpm2;
extern int tmp1,tmp2;
extern int wet1,wet2;
extern float longitude,latitude;
extern uint8_t num_satellites;
extern uint8_t fix_status_sats;
extern uint8_t sats_view;

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

#endif