#ifndef FIRMWARE_H
#define FIRMWARE_H


#include <stdbool.h>
#include <stdint.h>

// 상태 열거형 정의
typedef enum {
    STATE_MCU_INIT,
    STATE_IDLE,
    STATE_FW_REQUEST,
    STATE_FW_REQUEST_READY,
    STATE_FW_UPDATE,
    STATE_RESET
} FW_State;


typedef enum {
    EXT_REQUEST   = (uint32_t)0x60001200,  // 제어기 Write, Indigo Read
    EXT_RESPONSE  = (uint32_t)0x60001204,  // 제어기 Read, Indigo Write
    NEW_FW_EX_FSH = (uint32_t)0x00031000,  // 제어기 Write, Indigo Read (Ext Flash, 섹터 49)
    NEW_FW_RAM    = (uint32_t)0x60001208,  // 제어기 Don't care, Indigo W/R
    REQ_HW_RESET  = (uint32_t)0x60001210,  // 제어기 Read, Indigo Write
    DONT_CARE     = (uint32_t)0x00000000
} MemoryAddress;

 // 상태에 따라 구현체 호출
void setState(FW_State state);
// Helper Function
MemoryAddress getMemoryAdressByFWState(FW_State state);
// 구현체 
void openRequest(bool isOpenRequest);
bool getAuthorization();
void firmwareUpdate();
bool isResetRequested();
void resetDevice();
void testLogic(FW_State state);



#endif // FIRMWARE
