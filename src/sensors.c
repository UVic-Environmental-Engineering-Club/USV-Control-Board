#include "sensors.h"
#include "config.h"
#include "main.h"


static const char *TAG = "GPS: ";
#define TIME_ZONE (-8)   //Canada Time
#define YEAR_BASE (2000) //date in GPS starts from 2000
/**
 * @brief GPS Event Handler
 *
 * @param event_handler_arg handler specific arguments
 * @param event_base event base, here is fixed to ESP_NMEA_EVENT
 * @param event_id event id
 * @param event_data event specific arguments
 */
static void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    gps_t *gps = NULL;
    switch (event_id) {
    case GPS_UPDATE:
        gps = (gps_t *)event_data;
        /* print information parsed from GPS statements */
        ESP_LOGI(TAG, "%d/%d/%d %d:%d:%d => \r\n"
                 "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
                 "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
                 "\t\t\t\t\t\taltitude   = %.02fm\r\n",
                 gps->date.year + YEAR_BASE, gps->date.month, gps->date.day,
                 gps->tim.hour + TIME_ZONE, gps->tim.minute, gps->tim.second,
                 gps->latitude, gps->longitude, gps->altitude);
        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        //ESP_LOGW(TAG, "Unknown statement:%s", (char *)event_data);
        break;
    default:
        break;
    }
}

void GPSInit()
{
    /* NMEA parser configuration */
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    /* init NMEA parser library */
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
}

void accelerometer_run()
{
    
}

void GPS_run(nmea_parser_handle_t nmea_hdl)
{
    /* register event handler for NMEA parser library */
    nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);
}

void compass_run(i2c_cmd_handle_t cmd)
{
	//Set active registert to "Data Output X MSB Register"
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1);
	i2c_master_write_byte(cmd, 0x03, 1); //0x03 = "Data Output X MSB Register"
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	//Read values for X, Y and Z
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1);
	i2c_master_read_byte(cmd, data,   0); //"Data Output X MSB Register"
	i2c_master_read_byte(cmd, data+1, 0); //"Data Output X LSB Register"
	i2c_master_read_byte(cmd, data+2, 0); //"Data Output Z MSB Register"
	i2c_master_read_byte(cmd, data+3, 0); //"Data Output Z LSB Register"
	i2c_master_read_byte(cmd, data+4, 0); //"Data Output Y MSB Register"
	i2c_master_read_byte(cmd, data+5, 1); //"Data Output Y LSB Register "
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	short x = data[0] << 8 | data[1];
	short z = data[2] << 8 | data[3];
	short y = data[4] << 8 | data[5];
	int angle = atan2((double)y,(double)x) * (180 / 3.14159265) + 180; // angle in degrees
    angle -= 92; //Calibration
    if (angle > 359) angle -= 360;
    if (angle < 0) angle += 360;
	ESP_LOGE(tag, "angle: %d°, x: %duT, y: %duT, z: %duT", angle, x, y, z);
	vTaskDelay(1000/portTICK_PERIOD_MS);
	
}