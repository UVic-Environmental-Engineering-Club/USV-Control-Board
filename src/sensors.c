#include "sensors.h"
#include "i2c.h"
#include "config.h"

//For gps data
#define YEAR_BASE (2000) //date in GPS starts from 2000
#define TIME_ZONE (-8)   //Canada Time

//Function that describes what program will do when the GPS info is updated by the module
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
    case GPS_UPDATE: //Any time a new NMEA statement is received from the module
        gps = (gps_t *)event_data;
        /* print information parsed from GPS statements */
		if ((gps->tim.hour) + TIME_ZONE < 0)
			gps->tim.hour = gps->tim.hour + 24;
    

        ESP_LOGI(TAG, " %d:%d:%d => \r\n"
				 "\t\t\t\t\t\tsatellites in use   = %d\r\n"
                 "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
                 "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
                 "\t\t\t\t\t\taltitude   = %.02fm\r\n",
                 gps->tim.hour + TIME_ZONE, gps->tim.minute, gps->tim.second,
                 gps->sats_in_use, gps->latitude, gps->longitude, gps->altitude); 
        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        //ESP_LOGW(TAG, "Unknown statement:%s", (char *)event_data); 
        break;
    default:
        break;
    }
}

//Pass nmea handle to handler which then enters the event handler loop
void gps_handler_call(nmea_parser_handle_t nmea_hdl)
{
nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);	
}

//All the work gets done in gps_event_handler, this just adds a delay
void GPS_run()
{   
vTaskDelay(1000 / portTICK_PERIOD_MS);
}

//To be filled
void accelerometer_run()
{
    
}

//Compass Config
void compass_config(sensor_t mag)
{
    i2c_yeet(mag, MAG_CTRL_REG2, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG1, 0x54, MAG_REG_SIZE); //Reg config
    i2c_yeet(mag, MAG_CTRL_REG3, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG4, 0x00, MAG_REG_SIZE);
    i2c_yeet(mag, MAG_CTRL_REG5, 0x00, MAG_REG_SIZE);
    
}
//Lidar Config
void lidar_config(sensor_t LIDAR1, sensor_t LIDAR2, sensor_t LIDAR3)
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

//Pull raw xyz magnetic data from compass for sending to pi
void compass_run(sensor_t mag)
{
    const char magtag[] = "Magnetometer: ";
    byte status_reg = 0;
    byte magx1 = 0;
    byte magx2 = 0;
    byte magy1 = 0;
    byte magy2 = 0;
    byte magz1 = 0;
    byte magz2 = 0;

    i2c_yoink(mag, MAG_STATUS_REG, &status_reg, MAG_REG_SIZE); //Copy contents of status reg for comparison
    // if (status_reg & 0x03){ //Look for an update
    //     printf("Data available...collecting\n");
    // } else {
    //     printf("No new data available");
    // }

    i2c_yoink(mag, MAG_OUT_X_L, &magx1, MAG_REG_SIZE); //Pull XYZ data
    i2c_yoink(mag, MAG_OUT_X_H, &magx2, MAG_REG_SIZE);
    i2c_yoink(mag, MAG_OUT_Y_L, &magy1, MAG_REG_SIZE);
    i2c_yoink(mag, MAG_OUT_Y_H, &magy2, MAG_REG_SIZE);
    i2c_yoink(mag, MAG_OUT_Z_L, &magz1, MAG_REG_SIZE);
    i2c_yoink(mag, MAG_OUT_Z_H, &magz2, MAG_REG_SIZE);
    int16_t magx = ((magx2 << 8) | magx1); //Return raw values
    int16_t magy = ((magy2 << 8) | magy1);
    int16_t magz = ((magz2 << 8) | magz1);

    // ESP_LOGI(magtag, " mag-x: %d\tmag-y: %d\tmag-z:%d\n", magx, magy, magz);
  
    vTaskDelay(1000/ portTICK_RATE_MS);
}

//Pull raw lidar data
void lidar_run(sensor_t LIDAR1, sensor_t LIDAR2, sensor_t LIDAR3)
{
	byte lidar_val1;
	byte lidar_val2;
	int lidar1_dist = 0;
  int lidar2_dist = 0;
  int lidar3_dist = 0;

	//LIDAR1 acquisition
    lidar_val1 = 0x01;
    lidar_val2 = 0x00;
    while(lidar_val1 & 0x01 && lidar_val2 != 0x0a){     //will make 10 checks to see if lidar is ready to transmit before timing out. lidar is ready when bit 0 of register 0x01 is 0. If lidar is not connected properly lidar_dist will be very large, and take 5+ seconds to run through
        i2c_yeet(LIDAR1,0x00,0x04,1);
        i2c_yoink(LIDAR1,0x01,&lidar_val1,1);
        lidar_val2++;
    }
    i2c_yoink(LIDAR1,0x0F,&lidar_val1,1);
    i2c_yoink(LIDAR1,0x10,&lidar_val2,1);
    lidar1_dist = (lidar_val1 << 8) | lidar_val2;

    //LIDAR2 acquisition
    lidar_val1 = 0x01;
    lidar_val2 = 0x00;
    while(lidar_val1 & 0x01 && lidar_val2 != 0x0a){     //will make 10 checks to see if lidar is ready to transmit before timing out. lidar is ready when bit 0 of register 0x01 is 0. If lidar is not connected properly lidar_dist will be very large, and take 5+ seconds to run through
        i2c_yeet(LIDAR2,0x00,0x04,1);
        i2c_yoink(LIDAR2,0x01,&lidar_val1,1);
        lidar_val2++;
    }
    i2c_yoink(LIDAR2,0x0F,&lidar_val1,1);
    i2c_yoink(LIDAR2,0x10,&lidar_val2,1);
    lidar2_dist = (lidar_val1 << 8) | lidar_val2;

	//LIDAR3 acquisition
    lidar_val1 = 0x01;
    lidar_val2 = 0x00;
    while(lidar_val1 & 0x01 && lidar_val2 != 0x0a){     //will make 10 checks to see if lidar is ready to transmit before timing out. lidar is ready when bit 0 of register 0x01 is 0. If lidar is not connected properly lidar_dist will be very large, and take 5+ seconds to run through
        i2c_yeet(LIDAR3,0x00,0x04,1);
        i2c_yoink(LIDAR3,0x01,&lidar_val1,1);
        lidar_val2++;
    }
    i2c_yoink(LIDAR3,0x0F,&lidar_val1,1);
    i2c_yoink(LIDAR3,0x10,&lidar_val2,1);
    lidar3_dist = (lidar_val1 << 8) | lidar_val2;
    vTaskDelay(1000/ portTICK_RATE_MS);
}