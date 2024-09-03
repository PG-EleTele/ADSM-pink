#include "Lidar.h"

//////////////////////////////// PUBLIC  ////////////////////////////////

uint8_t Lidar_CalculateCRC8(uint8_t* framePtr){
    return Lidar_CalCRC8(framePtr, sizeof(Lidar_FrameTypeDef)-1);
}

bool Lidar_CheckCRC(const Lidar_FrameTypeDef *frame){
    int counted_crc = Lidar_CalCRC8((void*)frame, sizeof(Lidar_FrameTypeDef)-1);
    return frame->crc8 == counted_crc;
}

//////////////////////////////// PRIVATE ////////////////////////////////

uint8_t Lidar_CalCRC8(uint8_t *p, uint8_t len){
    uint8_t crc = 0;
    uint16_t i;
    for (i = 0; i < len; i++){
        crc = Lidar_CrcTable[(crc ^ *p++) & 0xff];
    }
    return crc;
}