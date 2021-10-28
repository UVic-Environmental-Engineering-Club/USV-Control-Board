#include "main.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO          (5)
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT
#define LEDC_DUTY               (2000)
#define LEDC_FREQUENCY          (50)

#define ADC2_CHAN0          ADC2_CHANNEL_0
#define ADC_ATTEN           ADC_ATTEN_DB_11


int raw_voltage = 0;
float duty = 0;

void task(void *parameter) 
{
	while(1) {

        adc2_get_raw( ADC2_CHANNEL_0, ADC_WIDTH_12Bit, &raw_voltage);

        duty = (float)raw_voltage / 4096.0 * 25.5 + 6.375;

        printf("%f\n", duty);

		ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, (int)duty);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
		vTaskDelay(20/portTICK_PERIOD_MS);
    
	}

}


void app_main()
{

    PWM_init();
    ADC_init();

    xTaskCreate(task,"task",2048,NULL,1,NULL);

}

void PWM_init()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 1638,
        .hpoint         = 0
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void ADC_init()
{
    adc2_config_channel_atten( ADC2_CHANNEL_0, ADC_ATTEN_0db );
}