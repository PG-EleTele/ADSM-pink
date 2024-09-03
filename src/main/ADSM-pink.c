#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "LidarTask.h"

//#define DEBUG

// 
void Init(void)
{   
#ifdef DEBUG
    printf("Init\n");
#endif
    Lidar_Init();
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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    Init();
    
    xTaskCreate(PrintAngleTable_Task, "printing_angle_table", 1024 * 2, NULL, 2, NULL);
    Lidar_CreateTask();
}
