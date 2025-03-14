#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*
 * author: datamining7830@gmail.com
 * Unit Test: Uart Commnad Test
 * Purpose: 파일명 UART 받기 테스트
 * Description: 
 * 1. 명령어 입력 시, 명령어와 데이터를 구분하여 처리
 * 2. 명령어 입력 시, 플레시 영역의 데이터를 읽음
 * EX) /help -> 명령어만 입력
 * EX) /GET 000000 (Flash Memory Read) -> 명령어와 데이터 입력
 */



#define RING_BUFFER_SIZE 256
#define DELIMETER_SPACE (0x20) // 스페이스 아스키
  /* Uart 입력 시, State 머신 */
typedef struct comms_packet_t {
    char operationCode[8];          // 명령어
    char data[RING_BUFFER_SIZE];    // 데이터
  } comms_packet_t;
  
  /* Uart 입력 시, State 머신 */
  typedef enum comms_state_t {
    CommsState_Operation,       //동작 Code
    CommsState_Data,            //Receive Data 
    CommsState_Process,         //받은 데이터 처리
  } comms_state_t;


static comms_packet_t temporary_packet = { .operationCode = {0}, .data = {0}}; // Uart Data 들어옴 [명령어][데이터 - optional]
static comms_state_t state = CommsState_Operation; // 초기 상태 - 명령어 받는 상태


/* 패킷 구조체 */
volatile uint8_t operationCnt = 0; // 받은 명령어 Char 수
volatile uint8_t dataCnt = 0; // 받은 데이터 char 수

//  대문자 -> 소문자 변경
uint8_t charToLower(char c){
    return  (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

/* UART에서 받은 16진수 데이터 처리 
* ex) 'A' -> 10 , 'a' -> 10 
*/
uint8_t hexCharToByte(char c) {
    return (c >= '0' && c <= '9') ? (c - '0') :
           (c >= 'A' && c <= 'F') ? (c - 'A' + 10) :
           (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : 0;
}




void process_uart_data(void) {
	switch(temporary_packet.operationCode[0]){
			
		case 'g' : // 파일 이름
            if(strcmp(temporary_packet.operationCode, "get")){
                // 데이터 처리
                uint32_t u32_addr = strtol(temporary_packet.data, NULL, 16);
                uint8_t u8_addr[3] = {0};
                u8_addr[0] = (u32_addr >> 16) & 0xFF;
                u8_addr[1] = (u32_addr >> 8) & 0xFF;
                u8_addr[2] = (u32_addr) & 0xFF;
                printf("Address: %02X %02X %02X\r\n",u8_addr[0],u8_addr[1],u8_addr[2]);
            }

        break;

		default:{
			printf("Invalid Command\r\n");
		}break;
	}

    memset(temporary_packet.operationCode,'\0', sizeof(temporary_packet.operationCode)); // 데이터 초기화
    memset(temporary_packet.data,'\0', sizeof(temporary_packet.data)); // 데이터 초기화
	
	operationCnt = 0, dataCnt = 0; // 초기화
}


/* 데이터 받을 시 마다 State 변경 */
void uart_data_stateMachine(char receivedChar){
	switch (state) {
		  case CommsState_Operation: {
			  uint8_t data = receivedChar;
			   if((data == '\n')){ // 데이터 입력 없고 명령어만 입력
                  // temporary_packet.data[dataCnt] = '\0';  //!  문자열 종료
				   process_uart_data(); // 명령어만 처리 ex) /list\n 
			   }
			   else{
				  temporary_packet.operationCode[operationCnt++] = charToLower(data);
				  if (data == DELIMETER_SPACE) { // 스페이스 받으면 데이터 받는 상태로 변경
					  state = CommsState_Data;
				  }
			  }
		  } break;
		  case CommsState_Data: {
			  uint8_t data = receivedChar;
			  if (data == DELIMETER_SPACE){
				  break;
			  }
			  else if (data == '\n') {  // 줄 바꿀시
					 process_uart_data();
					 state = CommsState_Operation;
			  }else{
            
				temporary_packet.data[dataCnt] = data;
                printf("Data[%d]: %c\r\n",dataCnt,temporary_packet.data[dataCnt]);
                dataCnt = dataCnt+1;
			  }
		  } break;
		  default: {
			  state = CommsState_Operation;
		  } break;
		}
}

int main(){
    // 입력을 받을 시, event 발생
    bool isExit = false;
    while(!isExit){  
        // 입력 받기 
        char input = getchar();
        uart_data_stateMachine(input);
        if(input == 'q'){
            isExit = true;
        }
    }
    printf("프로그램 종료");
    return 0;
}
