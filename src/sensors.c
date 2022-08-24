#include "sensors.h"
#include "rtos.h"
//For gps data
#define YEAR_BASE (2000) //date in GPS starts from 2000
#define TIME_ZONE (-8)   //Canada Time


//char const *magtag = "Compass: ";

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
    
        longitude = gps->longitude;
        latitude = gps->latitude;
        num_satellites = gps->sats_in_use;
        fix_status_sats = gps->fix;
        sats_view = gps->sats_in_view;
        //printf("Actual GPS: Long: %09f\tLat: %09f\n", gps->longitude, gps->latitude);
        uart_flush_input(UART_NUM_1);
        uart_pattern_queue_reset(UART_NUM_1, 160);
        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        //ESP_LOGW(TAG, "Unknown GPS statement:%s", (char *)event_data); 
        break;
    default:
        break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

//Pass nmea handle to handler which then enters the event handler loop
void gps_handler_call(nmea_parser_handle_t nmea_hdl)
{
nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);	
}

//To be filled
void accelerometer_run()
{
    
}



//Pull raw xyz magnetic data from compass for sending to pi
void compass_run()
{
    i2c_yoink(mag, MAG_STATUS_REG, &status_reg, MAG_REG_SIZE); //Copy contents of status reg for comparison
    if (status_reg & 0x03){ //Look for an update
        i2c_yoink(mag, MAG_OUT_X_L, &magx1, MAG_REG_SIZE); //Pull XYZ data
        i2c_yoink(mag, MAG_OUT_X_H, &magx2, MAG_REG_SIZE);
        i2c_yoink(mag, MAG_OUT_Y_L, &magy1, MAG_REG_SIZE);
        i2c_yoink(mag, MAG_OUT_Y_H, &magy2, MAG_REG_SIZE);
        i2c_yoink(mag, MAG_OUT_Z_L, &magz1, MAG_REG_SIZE);
        i2c_yoink(mag, MAG_OUT_Z_H, &magz2, MAG_REG_SIZE);
        int16_t magx = ((magx2 << 8) | magx1); //Return raw values
        int16_t magy = ((magy2 << 8) | magy1);
        int16_t magz = ((magz2 << 8) | magz1);
        compass_x = magx;
        compass_y = magy;
        compass_z = magz;
    }
    vTaskDelay(1000/portTICK_RATE_MS);
}

//Pull raw lidar data
void lidar_run()
{
    lidar_val1 = 0x01;
	lidar_val2 = 0x00;
    while(lidar_val1 & 0x01 && lidar_val2 != 0x0a){     //will make 10 checks to see if lidar is ready to transmit before timing out. lidar is ready when bit 0 of register 0x01 is 0. If lidar is not connected properly lidar_dist will be very large, and take 5+ seconds to run through
        i2c_yeet(LIDAR1,0x00,0x04,1);
        i2c_yoink(LIDAR1,0x01,&lidar_val1,1);
        lidar_val2++;
    }
    i2c_yoink(LIDAR1,0x0F,&lidar_val1,1);
    i2c_yoink(LIDAR1,0x10,&lidar_val2,1);
    int16_t lidar1_dist = (lidar_val1 << 8) | lidar_val2;
    lid1 = lidar1_dist;

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
    int16_t lidar2_dist = (lidar_val1 << 8) | lidar_val2;
    lid2 = lidar2_dist;

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
    int16_t lidar3_dist = (lidar_val1 << 8) | lidar_val2;
    lid3 = lidar3_dist;

    vTaskDelay(1000/ portTICK_RATE_MS);
}