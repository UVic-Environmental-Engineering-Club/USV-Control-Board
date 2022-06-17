#include "rtos.h"

xTaskHandle UART_task;
xTaskHandle I2C_task;
xTaskHandle motorControl_task;
xTaskHandle GPS_task;




void RTOSInit(void)
{
    xTaskCreatePinnedToCore(UART_PRIVATETASK, "UART", 10000, NULL, 1, &UART_task, 1);
    xTaskCreatePinnedToCore(I2C_PRIVATETASK, "I2C", 10000, NULL, 1, &I2C_task, 1);
    xTaskCreatePinnedToCore(motorControl_PRIVATETASK, "motor control", 10000, NULL, 0, &motorControl_task, 0);
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

    sensor_t mag = {MAG_ID, MAG_ADDR, MAG_NUMREG};
    sensor_t LIDAR1 = {1,0x62,23};
    sensor_t LIDAR2 = {1,0x64,23};
    sensor_t LIDAR3 = {1,0x66,23};

    compass_config(mag);
    //lidar_config(LIDAR1, LIDAR2, LIDAR3);

    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        compass_run(mag);
        //lidar_run(LIDAR1, LIDAR2, LIDAR3);
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

