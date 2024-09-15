#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "LidarTask.h"
#include "MotorsControl.h"

//#define DEBUG

// 
void Init(void)
{   
#ifdef DEBUG
    printf("Init\n");
#endif
    Lidar_Init();
    Motors_Init();
}

/**
 * @brief This is an example task using lidar_task
 */
static void PrintAngleTable_Task(void *arg){
    while(1){
        // clear terminal
        printf("\033[H\033[J");

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

void Loop(){
    while(true){
        if(Lidar_GetAngle(0) > 300){
            Motors_SetSpeed(MOTOR_LEFT, 100, MOTOR_FORWARD);
            Motors_SetSpeed(MOTOR_RIGHT, 100, MOTOR_FORWARD);
        }
        else{
            Motors_SetSpeed(MOTOR_LEFT, 0, MOTOR_FORWARD);
            Motors_SetSpeed(MOTOR_RIGHT, 0, MOTOR_FORWARD);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{
    Init();

    //xTaskCreate(PrintAngleTable_Task, "printing_angle_table", 1024 * 2, NULL, 0, NULL);
    Lidar_CreateTask();

    Loop();

}
