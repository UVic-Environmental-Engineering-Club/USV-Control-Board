#define ACC_GYR_ID                  0
#define ACC_GYR_ADDR                0x68
#define ACC_GYR_REG_SIZE            1
#define ACC_GYR_WHO_AM_I_REG        0x75
#define ACC_GYR_ACC_X_1_REG         0x3B
#define ACC_GYR_ACC_X_2_REG         0x3C
#define ACC_GYR_PWRMGM_REG          0x6B
#define ACC_GYR_NUMREG              1

#include "i2c.h"

/*void app_main() 
{
    i2c_init();

    sensor_t acc_gyr;
    acc_gyr.id = ACC_GYR_ID;
    acc_gyr.addr = ACC_GYR_ADDR;
    acc_gyr.number_of_registers = ACC_GYR_NUMREG;

    byte who_am_i = 0;

    byte accelx1 = 0;
    byte accelx2 = 0;

    byte pwr = 0;

    i2c_yeet(acc_gyr, ACC_GYR_PWRMGM_REG, 0x00, ACC_GYR_REG_SIZE);

    while(1)
    {
        i2c_yoink(acc_gyr, ACC_GYR_WHO_AM_I_REG, &who_am_i, ACC_GYR_REG_SIZE);

        i2c_yoink(acc_gyr, ACC_GYR_ACC_X_1_REG, &accelx1, ACC_GYR_REG_SIZE);

        i2c_yoink(acc_gyr, ACC_GYR_ACC_X_2_REG, &accelx2, ACC_GYR_REG_SIZE);

        i2c_yoink(acc_gyr, ACC_GYR_PWRMGM_REG, &pwr, ACC_GYR_REG_SIZE);

        int accelx = (accelx1 << 8) | accelx2;
        

        printf("%X\n", who_am_i);
        printf("%X\n", pwr);
        printf("%d\n", accelx);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    } 
}
*/