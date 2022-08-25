#include "motorcontrol.h"

uint16_t angle_to_duty(uint8_t angle){

    if (angle > 180) //Don't send bad angles
        angle = 90;
    
    float fraction = angle/180.0; //Fractional multiplier
    uint16_t duty_val = floor(3276*fraction); //Converts above fraction to a duty value
    return (3276+duty_val); //Adds the duty value to a 1ms duty since servo is 1-2ms duty
}

void motor_run()
{
    turn_rudder();
}

void turn_rudder(void)
{
    PWM_duty_cycle_update(0, 12, angle_to_duty(ran));
    vTaskDelay (20 / portTICK_RATE_MS);
}

