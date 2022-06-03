#include "rtos.h"
#include "sensors.h"

xTaskHandle UART_task;
xTaskHandle I2C_task;
xTaskHandle motorControl_task;
xTaskHandle GPS_task;
xTaskHandle accelerometer_task;
xTaskHandle compass_task;
xTaskHandle lidar_task;



void RTOSInit(void)
{
    xTaskCreatePinnedToCore(UART_PRIVATETASK, "UART", 10000, NULL, 1, &UART_task, 1);
    xTaskCreatePinnedToCore(I2C_PRIVATETASK, "I2C", 10000, NULL, 1, &I2C_task, 1);
    xTaskCreatePinnedToCore(accelerometer_PRIVATETASK, "accelerometer", 10000, NULL, 0, &accelerometer_task, 0);
    xTaskCreatePinnedToCore(motorControl_PRIVATETASK, "motor control", 10000, NULL, 0, &motorControl_task, 0);
    xTaskCreatePinnedToCore(compass_PRIVATETASK, "compass", 10000, NULL, 0, &GPS_task, 0);
    xTaskCreatePinnedToCore(lidar_PRIVATETASK, "lidar", 10000, NULL, 0, &lidar_task, 0);
    GPS_Init();
}

void GPS_Init()
{
    /* NMEA parser configuration */
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    /* init NMEA parser library */
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    /* register event handler for NMEA parser library */
    gps_handler_call(nmea_hdl);    
}

void UART_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = UART_TASK_RUN_PERIOD / portTICK_PERIOD_MS;

    lastRunTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        UART_run();
    }
}

void I2C_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = I2C_TASK_RUN_PERIOD / portTICK_PERIOD_MS;

    lastRunTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        I2C_run();
    }
}

void accelerometer_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = ACCEL_TASK_RUN_PERIOD / portTICK_PERIOD_MS;

    lastRunTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        accelerometer_run();
    }
}

void GPS_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = GPS_TASK_RUN_PERIOD / portTICK_PERIOD_MS;
    
    lastRunTime = xTaskGetTickCount();
    
    
    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        GPS_run();
    }
}

void compass_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = COMPASS_TASK_RUN_PERIOD / portTICK_PERIOD_MS;

    lastRunTime = xTaskGetTickCount();
    i2c_cmd_handle_t cmdhandle = compass_config_run();
    
    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        compass_run(cmdhandle);
    }
}

void lidar_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = COMPASS_TASK_RUN_PERIOD / portTICK_PERIOD_MS;

    lastRunTime = xTaskGetTickCount();
    lidar_config();
    
    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        lidar_run();
    }
}

void motorControl_PRIVATETASK(void* params)
{
    TickType_t lastRunTime;
    TickType_t runPeriod = MOTOR_TASK_RUN_PERIOD / portTICK_PERIOD_MS;

    lastRunTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        motor_run();
    }
}

