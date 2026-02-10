#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include <string.h>
#include "user_input.h"


#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE     // Only mode available
#define LEDC_OUTPUT_IO          12                      // GPIO 12
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT       // 13 bit resolution
#define LEDC_FREQUENCY          50                      // 50 Hz = 1 / 20 ms
#define LOOP_DELAY_MS           2000                    // Update time (ms)


void app_main(void)
{
    // Minimum and maximum servo pulse widths in microseconds (us)
    int t_min_us = 750;
    int t_max_us = 2250;
   
    // LEDC counter full scale in bits (aka counts or duty) and in microseconds
    const int ledc_fs_bits = pow(2, LEDC_DUTY_RES);
    const int ledc_fs_us   = 1000000 / LEDC_FREQUENCY;
   
    // Conversion factor from microseconds to bits
    const float bits_per_us = (float)ledc_fs_bits / (float)ledc_fs_us;              
   
    // Convert the min and max pulse widths from microseconds to bits.
    uint32_t t_min_bits = t_min_us * bits_per_us;
    uint32_t t_max_bits = t_max_us * bits_per_us;
   
    // Prepare and apply the LEDC timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);
   
    // Prepare and apply the LEDC channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,    // No interrupt on completion
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = t_min_bits,           // Set initial angle
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
   
    // Create a task from the user_input function found in "user_input.h".
    xTaskCreate(user_input, "user_input", configMINIMAL_STACK_SIZE * 3,
        NULL, 5, NULL
    );
   
    // User inputs 0 to stop program.
    while (t_min_us != 0) {
       
        // When the task has a new string, read min and max microseconds
        // from the string, then signal this string has been used.
        if(string_ready){            
            sscanf(ret_string, "%d %d", &t_min_us, &t_max_us);
            string_ready = false;
        }
       
        // Convert from microseconds to bits.
        t_min_bits = t_min_us * bits_per_us;
        t_max_bits = t_max_us * bits_per_us;
       
        // Set counter bits for max angle and update to apply.
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, t_max_bits);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
       
        vTaskDelay(LOOP_DELAY_MS / portTICK_PERIOD_MS);  
       
        // Set counter bits for min angle and update to apply.
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, t_min_bits);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
       
        vTaskDelay(LOOP_DELAY_MS / portTICK_PERIOD_MS);  
    }


    task_over = true;
   
    printf("\n\nTerminated\n\n");
}
