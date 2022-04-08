#include "i2c.h"



esp_err_t i2c_yoink(sensor_t sensor, i2c_register_t i2c_register, byte* data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (sensor.addr << 1) | I2C_MASTER_WRITE, 0);
	i2c_master_write_byte(cmd, i2c_register, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (sensor.addr << 1) | I2C_MASTER_READ, 1);

	i2c_master_read_byte(cmd, data, 1);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	return (len);
}

esp_err_t i2c_yeet(sensor_t sensor, i2c_register_t i2c_register, byte data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (sensor.addr << 1) | I2C_MASTER_WRITE, 1);
	i2c_master_write_byte(cmd, i2c_register, 1);
	i2c_master_write_byte(cmd, data, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	return (len);
}

esp_err_t i2c_init(void)
{

    i2c_config_t conf = 
    {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,   
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(I2C_PORT, &conf);

    return i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}