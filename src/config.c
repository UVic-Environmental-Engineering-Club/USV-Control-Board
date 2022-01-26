#include "sensors.h"
#include "main.h"

i2c_cmd_handle_t compass_config_run(void)
{
    ESP_LOGD(tag, ">> hmc5883l");
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = PIN_SDA;
	conf.scl_io_num = PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
    // conf.clk_flags = 0;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_1, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_1, I2C_MODE_MASTER, 0, 0, 0));

	

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1);
	i2c_master_write_byte(cmd, 0x02, 1); // 0x02 = "Mode register"
	i2c_master_write_byte(cmd, 0x00, 1); // 0x00 = "Continuous-Measurement Mode"
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	//Set value in "Configuration Register B"
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1);
	i2c_master_write_byte(cmd, 0x01, 1); // 0x01 = "Configuration Register B"
	i2c_master_write_byte(cmd, 0x20, 1); // 0x20 = default Gain setting
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	//Set active register to "Identification Register A"
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, 10, 1)); //10 = 0x0A = "Identification Register A"
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_1, cmd, 100/portTICK_PERIOD_MS));
	i2c_cmd_link_delete(cmd);

	//Get data from Identification Register A, B and C
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1));
	i2c_master_read_byte(cmd, data,   0);
	i2c_master_read_byte(cmd, data+1, 0);
	i2c_master_read_byte(cmd, data+2, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_1, cmd, 100/portTICK_PERIOD_MS));
	i2c_cmd_link_delete(cmd);

    return cmd;
}


void UARTInit()
{
const uart_port_t uart_num = UART_NUM_0;
    uart_config_t uart_config = {                                //uart 0 config
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(uart_num, &uart_config);
    uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
}