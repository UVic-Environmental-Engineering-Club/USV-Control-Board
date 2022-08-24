#include "rtos.h"


xTaskHandle UART_task;
xTaskHandle I2C_task;
xTaskHandle motorControl_task;

sensor_t mag = {MAG_ID, MAG_ADDR, MAG_NUMREG};
sensor_t LIDAR1 = {1,0x62,23};
sensor_t LIDAR2 = {1,0x64,23};
sensor_t LIDAR3 = {1,0x66,23};

byte status_reg = 0;
byte magx1 = 0;
byte magx2 = 0;
byte magy1 = 0;
byte magy2 = 0;
byte magz1 = 0;
byte magz2 = 0;
int compass_x = 0;
int compass_y = 0;
int compass_z = 0;

byte lidar_val1 = 0x01;
byte lidar_val2 = 0x00;
int lid1 = 0;
int lid2 = 0;
int lid3 = 0;

float longitude = 0;
float latitude = 0;
uint8_t num_satellites = 0;
uint8_t fix_status_sats = 0; 
uint8_t sats_view = 0;

int rpm1,rpm2 = 0;
int tmp1,tmp2 = 0;
int wet1,wet2 = 0;
int gyrx,gyry,gyrz = 0;
int accelx,accely,accelz = 0;

void RTOSInit(void)
{
    xTaskCreatePinnedToCore(UART_PRIVATETASK, "UART", 10000, NULL, 0, &UART_task, 1);
    xTaskCreatePinnedToCore(I2C_PRIVATETASK, "I2C", 10000, NULL, 1, &I2C_task, 1);
    xTaskCreatePinnedToCore(motorControl_PRIVATETASK, "motor control", 10000, NULL, 1, &motorControl_task, 0);
    GPS_Init();
    UARTInit();
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

    compass_config();
    lidar_config();

    while(1)
    {
        vTaskDelayUntil(&lastRunTime, runPeriod);
        compass_run();
        //lidar_run();
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

