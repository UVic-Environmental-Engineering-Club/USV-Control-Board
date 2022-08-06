#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void PWM_config_init(ledc_timer_bit_t timer_resolution, int frequency, int timer_source, ledc_mode_t speed_mode, int gpio_number, int channel);
void PWM_duty_cycle_update(int channel, int gpio_num, int duty_cycle);
void PWM_duty_cycle_reset(int channel, int gpio_num);
void PWM_shutdown(void);
