#ifndef MOTOR
#define MOTOR

#include "PWM.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include <math.h>
#include "communications.h"

void motor_run(void);
void turn_rudder(void);
uint16_t angle_to_duty(uint8_t angle);

#endif