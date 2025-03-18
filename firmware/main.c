#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "firmware.h"



// UART , MAIN에서도 관리 해야하긴 함. 



extern StateMachine sm ;


void initFirmware(){
    sm = createStateMachine();
    sm.transition(&sm, STATE_MCU_INIT);
    sm.printState(&sm);


}

// main 함수에서는 IDLE 상태에서 동작 ,

int main() {
    // 상태 머신 객체 생성


    sm.printState(&sm);  // 현재 상태 출력
    sm.transition(&sm, STATE_IDLE);  // 상태 변경
    sm.printState(&sm);  // 변경된 상태 출력

    return 0;
}