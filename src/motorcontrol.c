#include "motorcontrol.h"


void motor_run()
{
    turn_rudder();
}


int get_on_my_lvl(float sliceOfCake, int CakeSize){

    /*Takes the value of how long the servo should be turning for (range: 0.5 - 2 ) given by sliceOfCake and then the timelength is given with CakeSize
    the function then return a fully calculated int value )*/

    sliceOfCake = pow(sliceOfCake, CakeSize);

    if(sliceOfCake >= 1){
        sliceOfCake = floor(sliceOfCake/10);
    }else{
        sliceOfCake = ceil(sliceOfCake/10);
    }
    

    return sliceOfCake;

}

void turn_rudder(void)
{
    PWM_duty_cycle_update(0, 12, ran);
    vTaskDelay (20 / portTICK_RATE_MS);
}

void rudderInit(void)
{
    PWM_config_init(LEDC_TIMER_20_BIT, 50, 0, LEDC_HIGH_SPEED_MODE, 12, 0);
    PWM_duty_cycle_update(0, 12, get_on_my_lvl(0.5,20));
}
