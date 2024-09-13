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

void app_main(void)
{
    Init();

    //xTaskCreate(PrintAngleTable_Task, "printing_angle_table", 1024 * 2, NULL, 0, NULL);
    Lidar_CreateTask();

    while(1){
        Motors_SetSpeed(true, 100, true);
        Motors_SetSpeed(false, 50, true);
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("right: %i\n", (int)Motors_encoderPulseCountMotorRight);
        printf("left: %i\n", (int)Motors_encoderPulseCountMotorLeft);

        //Motors_SetSpeed(true, 50, true);
        //Motors_SetSpeed(false, 50, true);
        //vTaskDelay(pdMS_TO_TICKS(5000));

        Motors_SetSpeed(true, 0, true);
        Motors_SetSpeed(false, 0, true);
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("right: %i\n", (int)Motors_encoderPulseCountMotorRight);
        printf("left: %i\n", (int)Motors_encoderPulseCountMotorLeft);
        //Motors_SetSpeed(true, 100, false);
        //Motors_SetSpeed(false, 100, false);
        //vTaskDelay(pdMS_TO_TICKS(5000));
    }

}
