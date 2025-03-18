#include <stdio.h>
#include "firmware.h"








// Logic Handling
void setState(FW_State state){
    bool isExit = false;
    while(!isExit){
        switch(state){
            case STATE_MCU_INIT:        
                printf("STATE_MCU_INIT\r\n");
                bool isNeedReste = isResetRequested();
                if(isNeedReste){
                    state = STATE_RESET;
                }else{
                    state = STATE_IDLE;
                }
                break;
            case STATE_IDLE:
                isExit = true;
                printf("STATE_IDLE..EXIT\r\n");
                break;
            case STATE_FW_REQUEST:
                openRequest(true);
                state = STATE_FW_REQUEST_READY;
                printf("STATE_FW_REQUEST\r\n");
                break;
            case STATE_FW_REQUEST_READY:
                while(!getAuthorization()){
                }
                state = STATE_FW_UPDATE;
                break;
            case STATE_FW_UPDATE:
                printf("STATE_FW_UPDATE\r\n");
                firmwareUpdate();
                state = STATE_IDLE;
                break;
            case STATE_RESET:
                printf("reset Device\r\n");
                resetDevice(); // reset Device.. 
                state = STATE_IDLE;
                break;
            default:
                printf("Invalid State\r\n");
                break;
        }
        printf("end\r\n");

    }
    
}


void openRequest(bool isOpenRequest){
    uint32_t handlingAddr = EXT_REQUEST; 
    uint8_t addr[3] = {0};
    //const
     
    addr[0] = (handlingAddr >> 16) & 0xFF;
    addr[1] = (handlingAddr >> 8) & 0xFF;
    addr[2] = (handlingAddr) & 0xFF;
    if(isOpenRequest){
        printf("Open Request\r\n"); // Do I2C Open
    }else{
        printf("Close Request\r\n"); // Do I2C Close
    }
}
bool getAuthorization(){

}
void firmwareUpdate(){

}
bool isResetRequested(){

}
void resetDevice(){

}