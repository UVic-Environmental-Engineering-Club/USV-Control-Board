#include "PWM.h"

/**********************************************************************************************************************************************
* This function sets up a GPIO pin to output a specific frequency with a duty cycle of 0
* timer resolution is the bit resolution of the PWM duty cycle
* timer source is the source of the channel (0 - 3)
* speed mode is either LEDC_HIGH_SPEED_MODE or LEDC_LOW_SPEED_MODE
* channel is the LEDC channel to use (0 - 7)
**********************************************************************************************************************************************/

void PWM_config_init(ledc_timer_bit_t timer_resolution, int frequency, int timer_source, ledc_mode_t speed_mode, int gpio_number, int channel){
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = timer_resolution, // resolution of PWM duty
        .freq_hz = frequency,                      // frequency of PWM signal
        .speed_mode = speed_mode,           // timer mode
        .timer_num = timer_source,            // selects the timer source of channel (0 - 3)
        .clk_cfg = LEDC_AUTO_CLK,              // Auto selects the source clock
    };

    /*
     * Prepare and set configuration of channel
     * that will be used by LED Controller
     */
    ledc_channel_config_t ledc_channel = {
        .speed_mode = speed_mode,     //LEDC speed mode HIGH or LOW
        .channel = channel,       // LEDC channel (0 - 7)
        .gpio_num = gpio_number,     // LEDC output gpio number
        .timer_sel = timer_source,     // selects the timer source of channel (0 - 3)
        .duty = 0       // [0, (2**duty_resolution)]
    };

    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);
}

/**********************************************************************************************************************************************
* This function updates the duty cycle of the PWM pin attached to the specific LEDC channel and GPIO
**********************************************************************************************************************************************/

void PWM_duty_cycle_update(int channel, int gpio_num, int duty_cycle){
    /*
     * Prepare and set configuration of channel
     * that will be used by LED Controller
     */
    ledc_channel_config_t ledc_channel = {
        .channel = channel,       // LEDC channel (0 - 7)
        .gpio_num = gpio_num,     // LEDC output gpio number
        .duty = duty_cycle       // [0, (2**duty_resolution)]
    };
    ledc_channel_config(&ledc_channel);
}

/**********************************************************************************************************************************************
* This function shuts down the duty cycle of the PWM pin attached to the specific LEDC channel and GPIO
**********************************************************************************************************************************************/

void PWM_duty_cycle_reset(int channel, int gpio_num){
    /*
     * Prepare and set configuration of channel
     * that will be used by LED Controller
     */
    ledc_channel_config_t ledc_channel = {
        .channel = channel,       // LEDC channel (0 - 7)
        .gpio_num = gpio_num,     // LEDC output gpio number
        .duty = 0       // [0, (2**duty_resolution)]
    };
    ledc_channel_config(&ledc_channel);
}

/**********************************************************************************************************************************************
* This function shuts down the duty cycle of all LEDC channels which theoretically shuts down all GPIOs PWMs
**********************************************************************************************************************************************/

void PWM_shutdown(){
    for (size_t i = 0; i < 8; i++)
    {
        ledc_channel_config_t ledc_channel = {
        .channel = i,       // LEDC channel (0 - 7)
        .duty = 0       // [0, (2**duty_resolution)]
        };
        ledc_channel_config(&ledc_channel);
    }  
}