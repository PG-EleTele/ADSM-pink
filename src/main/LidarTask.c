#include "LidarTask.h"

int angleTable[ANGLES_IN_FULL_ROTATION];
SemaphoreHandle_t xMutex[ANGLES_IN_FULL_ROTATION];

//////////////////////////////// PUBLIC  ////////////////////////////////

void Lidar_Init(){
    Lidar_InitUART();
    Lidar_ClearAnglesTable();
    Lidar_InitMutexes();
}

void Lidar_CreateTask(){
    xTaskCreate(Lidar_Task, "lidar_task", LIDAR_TASK_STACK_DEPTH, NULL, LIDAR_TASK_PRIORITY, NULL);
}

void Lidar_SetAngle(unsigned int angle, uint16_t distance){
    angle = angle % ANGLES_IN_FULL_ROTATION;
    if(xSemaphoreTake(xMutex[angle], BLOCKTIME_IF_BUSSY_SET)){
        angleTable[angle] = distance;
        xSemaphoreGive(xMutex[angle]);
        taskYIELD(); 
    }
}

int Lidar_GetAngle(int angle){
    //printf("angle before: %i,", angle);
    if(angle < 0){
        angle += 360;
    }
    //printf("after: %i\n", angle);
    int result = -2;
    angle = angle % ANGLES_IN_FULL_ROTATION;
    if(xSemaphoreTake(xMutex[angle], BLOCKTIME_IF_BUSSY_GET)){
        result = angleTable[angle];
        xSemaphoreGive(xMutex[angle]);
        taskYIELD(); 
    }
    return result;
}

//////////////////////////////// PRIVATE ////////////////////////////////

void Lidar_InitUART(){
    const uart_config_t uart_config = {
        .baud_rate = 230400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, LIDAR_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void Lidar_ClearAnglesTable(){
    for (int i = 0; i < ANGLES_IN_FULL_ROTATION; i++)
    {
        angleTable[i] = NOT_SET;
    }
}

void Lidar_InitMutexes(){
    for(int i = 0; i < ANGLES_IN_FULL_ROTATION; i++)
    {
        xMutex[i] = xSemaphoreCreateMutex();
        if (xMutex[i] == NULL) {
#ifdef DEBUG
            printf("failed creating mutex\n");
#endif
            return;
        }
    }
}

void Lidar_Task(void *arg)
{    
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
    while(1){
        Lidar_ReadFrames(data);
    }
    free(data);
}

void Lidar_ReadFrames(uint8_t* buffer){
    const int rxBytes = uart_read_bytes(UART_NUM_1, buffer, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
    if (rxBytes < 0) {
        return;
    }
    buffer[rxBytes] = 0;

    int index[FRAMES_NUBMER_IN_BUFFER];
    Lidar_FillIndexesTable(index, buffer);
    
#ifdef LOG_FRAME_INFO
    printf("=============================================\n");
    printf("indexes:\t");
    for(int i = 0; i < FRAMES_NUBMER_IN_BUFFER; i++ ){
        printf("%i\t", index[i]);
    }
    printf("\n");
#endif

    Lidar_FrameTypeDef frame = { 0 };
    for(int i = 0; i < FRAMES_NUBMER_IN_BUFFER && index[i] != NOT_SET; i++){
        memcpy((void*)&frame, (void*)&buffer[index[i]], sizeof(Lidar_FrameTypeDef));

#ifdef LOG_FRAME_INFO
        printf("\t\tindex: %i\n", index[i]);    
        PrintFrameInfo(&frame);
#endif
        if(Lidar_CheckCRC((void*)&frame))
        {
            Lidar_UpdateAngleTable(&frame);
        }
    }
#ifdef LOG_FRAME_INFO
    printf("=============================================\n");
#endif
}

void Lidar_FillIndexesTable(int* indexesTable, uint8_t* buffer){
    for(int i = 0; i < FRAMES_NUBMER_IN_BUFFER; i ++ ){
        indexesTable[i] = NOT_SET;
    }
    for(int i = 0, counter = 0; i < RX_BUF_SIZE && counter < FRAMES_NUBMER_IN_BUFFER; i++){
        if(buffer[i] == LIDAR_HEADER_BYTE && buffer[i+1] == LIDAR_VER_LEN_BYTE && i < sizeof(Lidar_FrameTypeDef) * FRAMES_NUBMER_IN_BUFFER){
            indexesTable[counter++] = i;
        }
    }
}

void Lidar_PrintFrameInfo(const Lidar_FrameTypeDef *frame){
    if(!Lidar_CheckCRC(frame)){
        printf("\033[31mWrong CRC!\033[0m\n");
        printf("\tcrc8: %i\n", frame->crc8);
        printf("\tcounted crc: %i\n", Lidar_CalculateCRC8((void*)frame));
        printf("skipped \n\n");
        return;
    }
    
    printf("speed: %i\n", frame->speed);
    printf("timestamp: %i\n", frame->timestamp);
    printf("start angle: %i.%i \n", frame->start_angle / 100, frame->start_angle % 100);
    printf("end_angle: %i.%i\n", frame->end_angle / 100, frame->end_angle % 100);

    for(uint8_t i_point = 0; i_point < (frame->ver_len & 0b00011111); i_point++){
        printf("[%i]: ", i_point);
        printf("\t dist: %i.%icm", frame->point[i_point].distance / 10, frame->point[i_point].distance % 10);
        printf("\tintensity: %i\n", frame->point[i_point].intensity);
    }
    printf("\n");
}


void Lidar_UpdateAngleTable(Lidar_FrameTypeDef *frame){
    uint16_t step = (frame->end_angle - frame->start_angle)/(LIDAR_POINT_PER_PACK - 1);

#ifdef LOG_UPDATES
    printf("step: %i\n", step);
#endif

    for(int i = 0; i < LIDAR_POINT_PER_PACK; i++){
        // TODO: dodać intensity!!!
        uint16_t rounded = round((float)(frame->start_angle + step * i) / 100);

#ifdef LOG_UPDATES
        printf("\t%i, rounded: %i\n", frame->start_angle + step * i, rounded);
#endif

        if(i > 0 && round((float)(frame->start_angle + step * (i - 1)) / 100) == rounded)
        {
            uint16_t current = abs(frame->start_angle + step * i - rounded*100);
            uint16_t last = abs(frame->start_angle + step * (i-1) - rounded*100);
            
            if(current < last){
                //angleTable[rounded % ANGLES_IN_FULL_ROTATION] = frame->point[i].distance;
                Lidar_SetAngle(rounded % ANGLES_IN_FULL_ROTATION, frame->point[i].distance);

#ifdef LOG_UPDATES
                printf("update: %i\twith value: %i\n", rounded % ANGLES_IN_FULL_ROTATION, frame->point[i].distance);
#endif
            }
        }
        else{
            //angleTable[rounded % ANGLES_IN_FULL_ROTATION] = frame->point[i].distance;
            Lidar_SetAngle(rounded % ANGLES_IN_FULL_ROTATION, frame->point[i].distance);
#ifdef LOG_UPDATES
            printf("set: %i\twith value: %i\n", rounded % ANGLES_IN_FULL_ROTATION, frame->point[i].distance);
#endif
        }
        
    }
}