#ifndef MOTORS_CONTROL_H
#define MOTORS_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

//#define MOTOR_LOG

#define MOTOR_TIMER              LEDC_TIMER_0
#define MOTOR_MODE               LEDC_LOW_SPEED_MODE
#define MOTOR_CHANNEL_LEFT       LEDC_CHANNEL_0
#define MOTOR_CHANNEL_RIGHT      LEDC_CHANNEL_1
#define MOTOR_DUTY_RES           LEDC_TIMER_13_BIT  // PWM Resolution (13-bit)
#define MOTOR_DUTY               (8191)             // Max 2^13-1
#define MOTOR_FREQUENCY          (5000)             // PWM Frequency in Hz (5 kHz)

#define MOTOR_LEFT_PWM_PIN              (13)
#define MOTOR_RIGHT_PWM_PIN             (12)
#define MOTOR_LEFT_DIRECTION_PIN        (11)
#define MOTOR_RIGHT_DIRECTION_PIN       (10)
#define MOTOR_LEFT_ENCODER_PIN          (9)
#define MOTOR_RIGHT_ENCODER_PIN         (6)

#define MOTOR_LEFT true
#define MOTOR_RIGHT false
#define MOTOR_FORWARD true
#define MOTOR_BACKWARDS false

#define FORWARD_SPEED 100
#define TURN_SPEED 25

#define FREE_SPACE_ANGLE 15
#define FREE_SPACE_DISTANCE 300
#define FREE_SPACE_THRESHOLD 500

//////////////////////////////// PUBLIC  ////////////////////////////////
/**
 * @brief Initializes motors module in project
 */
void Motors_Init();
/**
 * @brief sets wheel speed on motors
 * @param isLeft determines which wheel you want to set, true is left, false is right
 * @param speed sets wheel speed value from min value to max [0,100]
 * @param direction determines if wheel is rotating forward or backward, true is forward, false is backward
 */
void Motors_SetSpeed(bool isLeft, int speed, bool direction);

/**
 * @brief Getter for Motors_encoderPulseCountMotorLeft
 */
int32_t Motors_GetEncoderCounterLeft();

/**
 * @brief Getter for Motors_encoderPulseCountMotorRight
 */
int32_t Motors_GetEncoderCounterRight();

//////////////////////////////// PRIVATE ////////////////////////////////
void Motos_InitTimer();
void Motors_InitChannels();
void Motors_InitDirection();
void Motors_SetLeftDirection(bool direction);
void Motors_SetRightDirection(bool direction);
void IRAM_ATTR Motors_EncoderIsrHandlerMotorLeft(void *arg);
void IRAM_ATTR Motors_EncoderIsrHandlerMotorRight(void *arg);
void Motors_InitEncoderInterruptLeft();
void Motors_InitEncoderInterruptRight();
void Motors_TurnLeft();
void Motors_TurnRight();

extern volatile int32_t Motors_encoderPulseCountMotorLeft;
extern volatile int32_t Motors_encoderPulseCountMotorRight;

#endif // !MOTORS_CONTROL_H