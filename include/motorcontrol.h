#ifndef MOTOR
#define MOTOR

#include "PWM.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include <math.h>
#include "communications.h"

extern int ran;
extern int mpr;

void motor_run(void);
int get_on_my_lvl(float sliceOfCake, int CakeSize);
void turn_rudder(void);
void rudderInit(void);

#endif