#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "LidarTask.h"
#include "MotorsControl.h"

//#define DEBUG

#define FAIL_OBSTACLE_POSITION -1000

/**
 * @brief Main robot loop. In this function is logic for controlling motors and lidar.
 */
void Loop();

/**
 * @brief Initializes all modules in project.
 */
void Init(void);

/**
 * @brief This is an example task using lidar_task
 */
static void PrintAngleTable_Task(void *arg);

/**
 * @brief Check if there is an obstacle at given angle in front of robot.
 * @returns true if there is an obstacle, false if there is free space in front of robot.
 */
bool IsObstacleInFront();

/**
 * @brief Returns position of obstacle if there is an obstacle in front of robot with given max angle.
 * @returns position of obstacle in angles. Returns fail if no one is found
 */
int GetObstacleInFrontPosition();

/**
 * @brief Checks surroundings and returns first free space starting from zero angle and jumps left to right by 1 degree.
 * @returns angle with free space.
 */
int CheckSurroundings();



void app_main(void)
{
    Init();

    //xTaskCreate(PrintAngleTable_Task, "printing_angle_table", 1024 * 2, NULL, 0, NULL);
    Lidar_CreateTask();

    Loop();

}

void Loop(){
    while(true){
        if(!IsObstacleInFront()){
            Motors_SetSpeed(MOTOR_LEFT, FORWARD_SPEED, MOTOR_FORWARD);
            Motors_SetSpeed(MOTOR_RIGHT, FORWARD_SPEED, MOTOR_FORWARD);
        }
        else{
            int newAngle = CheckSurroundings();

#ifdef DEBUG
            printf("new angle: %i\n", newAngle);
#endif
            if(newAngle < 0){
                Motors_TurnLeft();
            }
            else if(newAngle > 0){
                Motors_TurnRight();
            }
            else{
                int obstaclePos = GetObstacleInFrontPosition();
                if(obstaclePos != FAIL_OBSTACLE_POSITION){
                    if(obstaclePos > 0){
                        Motors_TurnLeft();
                    }
                    else if(obstaclePos < 0){
                        Motors_TurnRight();
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Init(void)
{   
#ifdef DEBUG
    printf("Init\n");
#endif
    Lidar_Init();
    Motors_Init();
}

static void PrintAngleTable_Task(void *arg){
    while(1){
        // clear terminal
        //printf("\033[H\033[J");

        // print table 
        for (int i = 1; i <= ANGLES_IN_FULL_ROTATION; i++)
        {
            uint16_t value = Lidar_GetAngle(i);
            printf("%03d: %i.%i", i % ANGLES_IN_FULL_ROTATION, value / 10, value % 10);
            if(i % 20 == 0){
                printf("\n");
            }
            else{
                printf("\t");
            }
        }
        // freeze screen and thread
        //vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

bool IsObstacleInFront(){
    for(int angle = -FREE_SPACE_ANGLE; angle <= FREE_SPACE_ANGLE; angle++){
        if(Lidar_GetAngle(angle) < FREE_SPACE_DISTANCE){
            return true;
        }
    }
    return false;
}

int GetObstacleInFrontPosition(){
    for(int angle = 1; angle <= FREE_SPACE_ANGLE; angle++){
        if(Lidar_GetAngle(-angle) < FREE_SPACE_DISTANCE){
            return -angle;
        }
        if(Lidar_GetAngle(angle) < FREE_SPACE_DISTANCE){
            return angle;
        }
    }
    return FAIL_OBSTACLE_POSITION;
}

int CheckSurroundings(){
    for(int angle = 0; angle < 180;angle++){
        if(Lidar_GetAngle(-angle) > FREE_SPACE_THRESHOLD){
            return -angle;
        }
        else if(Lidar_GetAngle(angle) > FREE_SPACE_THRESHOLD){
            return angle;
        }
    }
#ifdef DEBUG
    printf("problem: no free space\n");
#endif
    return FAIL_OBSTACLE_POSITION;
}

