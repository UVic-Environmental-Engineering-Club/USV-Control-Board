#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21      
#define I2C_MASTER_NUM              0                           
#define I2C_MASTER_FREQ_HZ          400000                                            
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_MASTER_RX_BUF_SIZE      0
#define I2C_MASTER_TX_BUF_SIZE      0
#define I2C_PORT                    0

typedef int i2c_register_t;

typedef unsigned char byte;

typedef struct sensor_t
{
    int id;
    byte addr;
    int number_of_registers;
} sensor_t;

esp_err_t i2c_init(void);
esp_err_t i2c_yoink(sensor_t sensor, i2c_register_t i2c_register, byte* data, size_t len);
esp_err_t i2c_yeet(sensor_t sensor, i2c_register_t i2c_register, byte data, size_t len);