#include "config.h"
#include "rtos.h" //Extern variables declared here
#include "sensors.h" //Handler call function here

void UARTInit()
{
    const uart_port_t uart_num = UART_NUM_0;
        uart_config_t uart_config = {                                //uart 0 config
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        };
        uart_param_config(uart_num, &uart_config);
        uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
}
//GPS Initialization
void GPS_Init()
{
    /* NMEA parser configuration */
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    /* init NMEA parser library */
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    /* register event handler for NMEA parser library */
    gps_handler_call(nmea_hdl);    
}
//Compass Config
void compass_config()
{
    i2c_yeet(mag, MAG_CTRL_REG2, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG1, 0x54, MAG_REG_SIZE); //Reg config
    i2c_yeet(mag, MAG_CTRL_REG3, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG4, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG5, 0x00, MAG_REG_SIZE);
    
}
//Lidar Config
void lidar_config()
{
    i2c_yeet(LIDAR1,0x18,(byte)8,1);                  //switches lidar2's address from default to 0x64 by reading it's 16 bit serial number into registers 0x18 & 0x19 etc.
    i2c_yeet(LIDAR1,0x19,(byte)38,1);
    i2c_yeet(LIDAR1,0x1a,(LIDAR2.addr<<1),1);
    i2c_yeet(LIDAR1,0x1e,0x10,1);
    i2c_yeet(LIDAR2,0x1e,0x18,1);
	i2c_yeet(LIDAR1,0x18,(byte)8,1);                  //switches lidar3's address from default to 0x66 by reading it's 16 bit serial number into registers 0x18 & 0x19 etc.
    i2c_yeet(LIDAR1,0x19,(byte)34,1);
    i2c_yeet(LIDAR1,0x1a,(LIDAR3.addr<<1),1);
    i2c_yeet(LIDAR1,0x1e,0x10,1);
    i2c_yeet(LIDAR2,0x1e,0x18,1);
}
