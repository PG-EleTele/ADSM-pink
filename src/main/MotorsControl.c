#include "MotorsControl.h"

volatile int32_t Motors_encoderPulseCountMotorLeft = 0;
volatile int32_t Motors_encoderPulseCountMotorRight = 0;

void Motors_Init(){
    Motos_InitTimer();
    Motors_InitChannels();
    Motors_InitDirection();
    Motors_InitEncoderInterruptLeft();
    Motors_InitEncoderInterruptRight();
}

void Motors_SetSpeed(bool isLeft, int speed, bool direction)
{
#ifdef MOTOR_LOG   
    printf("Speed: %i\n", speed);
#endif
    int duty = MOTOR_DUTY * (100 - speed) / 100;
    if(isLeft){
        Motors_SetLeftDirection(direction);
        ESP_ERROR_CHECK(ledc_set_duty(MOTOR_MODE, MOTOR_CHANNEL_LEFT, duty));
        ESP_ERROR_CHECK(ledc_update_duty(MOTOR_MODE, MOTOR_CHANNEL_LEFT));
    }
    else{
        Motors_SetRightDirection(direction);
        ESP_ERROR_CHECK(ledc_set_duty(MOTOR_MODE, MOTOR_CHANNEL_RIGHT, duty));
        ESP_ERROR_CHECK(ledc_update_duty(MOTOR_MODE, MOTOR_CHANNEL_RIGHT));
    }
}

void Motos_InitTimer(){
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = MOTOR_MODE,
        .timer_num        = MOTOR_TIMER,
        .duty_resolution  = MOTOR_DUTY_RES,
        .freq_hz          = MOTOR_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}

void Motors_InitChannels(){
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = MOTOR_MODE,
        .channel        = MOTOR_CHANNEL_LEFT,
        .timer_sel      = MOTOR_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_LEFT_PWM_PIN,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ledc_channel_config_t ledc_channel2 = {
        .speed_mode     = MOTOR_MODE,
        .channel        = MOTOR_CHANNEL_RIGHT,
        .timer_sel      = MOTOR_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_RIGHT_PWM_PIN,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));
}

void Motors_InitDirection()
{
    gpio_reset_pin(MOTOR_LEFT_DIRECTION_PIN);
    gpio_set_direction(MOTOR_LEFT_DIRECTION_PIN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(MOTOR_RIGHT_DIRECTION_PIN);
    gpio_set_direction(MOTOR_RIGHT_DIRECTION_PIN, GPIO_MODE_OUTPUT);
}

void Motors_InitEncoderInterruptLeft() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << MOTOR_LEFT_ENCODER_PIN),
        .pull_down_en = 0,
        .pull_up_en = 1
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(MOTOR_LEFT_ENCODER_PIN, Motors_EncoderIsrHandlerMotorLeft, NULL);
}

void Motors_InitEncoderInterruptRight() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << MOTOR_RIGHT_ENCODER_PIN),
        .pull_down_en = 0,
        .pull_up_en = 1
    };
    gpio_config(&io_conf);

    gpio_isr_handler_add(MOTOR_RIGHT_ENCODER_PIN, Motors_EncoderIsrHandlerMotorRight, NULL);
}

void IRAM_ATTR Motors_EncoderIsrHandlerMotorLeft(void *arg) {
    Motors_encoderPulseCountMotorLeft++;
}

void IRAM_ATTR Motors_EncoderIsrHandlerMotorRight(void *arg) {
    Motors_encoderPulseCountMotorRight++;
}

void Motors_SetLeftDirection(bool direction)
{
    gpio_set_level(MOTOR_LEFT_DIRECTION_PIN, direction);
}

void Motors_SetRightDirection(bool direction)
{
    gpio_set_level(MOTOR_RIGHT_DIRECTION_PIN, !direction);
}