#ifndef LIDAR_TASK_H
#define LIDAR_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "math.h"
#include "freertos/semphr.h"

#include "Lidar.h"

//#define LOG_UPDATES
//#define LOG_READ

#define LIDAR_RXD_PIN (GPIO_NUM_18)

// Task Settings
static const int LIDAR_TASK_PRIORITY = configMAX_PRIORITIES - 1;

#ifdef DEBUG
static const int LIDAR_TASK_STACK_DEPTH = 1024 * 4;
#else
static const int LIDAR_TASK_STACK_DEPTH = 1024 * 2;
#endif

#define BLOCKTIME_IF_BUSSY_SET 0 // min = 0, max = portMAX_DELAY
#define BLOCKTIME_IF_BUSSY_GET 0 // min = 0, max = portMAX_DELAY

#define FRAMES_NUBMER_IN_BUFFER 5
#define RX_BUF_SIZE (47 * (FRAMES_NUBMER_IN_BUFFER + 1))
#define NOT_SET -1
#define ANGLES_IN_FULL_ROTATION 360

extern int angleTable[ANGLES_IN_FULL_ROTATION];
extern SemaphoreHandle_t xMutex[ANGLES_IN_FULL_ROTATION];

// PUBLIC:

/**
 * @brief Initializes Lidar module in project
 */
void Lidar_Init();

/**
 * @brief Creates Lidar Task
 */
void Lidar_CreateTask();

/**
 * @brief sets distance in correct cell in anglesTable fot entered angle
 * @param angle index of cell in table corresponding to the angle in degrees [0,360]
 * @param distance value to set in table in mm
 */
void Lidar_SetAngle(unsigned int angle, uint16_t distance);

/**
 * @brief returns distance at the entered angle
 * @param angle index of cell in table corresponding to the angle in degrees [0, 360]
 * @return distance at this angle in mm
 */
int Lidar_GetAngle(unsigned int angle);


// PRIVATE:
void Lidar_InitUART();
void Lidar_ClearAnglesTable();
void Lidar_InitMutexes();
void Lidar_Task(void *arg);
void Lidar_ReadFrames(uint8_t* buffer);
void Lidar_FillIndexesTable(int* indexesTable, uint8_t* buffer);
void Lidar_PrintFrameInfo(const Lidar_FrameTypeDef *frame);
void Lidar_UpdateAngleTable(Lidar_FrameTypeDef *frame);

#endif // !LIDAR_TASK_H