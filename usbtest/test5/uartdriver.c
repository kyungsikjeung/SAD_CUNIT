#include <stdio.h>
#include <string.h>
#include "uartdriver.h"
#include "usb.h"


/*
 * author: datamining7830@gmail.com
 * Unit Test: UART Driver에서 받은 파일 이름 저장
 * Purpose: 
 * Description: 
 */



static char file_name[256];

void setFileName(const char* name) {
    strncpy(file_name, name, sizeof(file_name) - 1);
    file_name[sizeof(file_name) - 1] = '\0'; // Null -Terminate 추가
}

const char* get_file_name() {
    return file_name;
}

// gcc -o test5.exe uartdriver.c usb.c 
// 실행 예시: ./test5 "test123.txt"
int main(int argc, char* argv[]) {
    printf("Hello from test5\n");

    if(file_name[0] == '\0'){
        printf("file_name is NULL\n");
    }else{
        printf("file_name is not NULL\n");
    }

    // 파일 이름 입력 안했을 시 
    if (argc < 2) {
        printf("입력받은 데이터: %s <파일 이름>\n", argv[0]);
        return 1; // 오류 종료
    }

    // 테스트 
    printf("입력한 파일 이름: %s\n", argv[1]);
    setFileName(argv[1]);

    if(file_name[0] == '\0'){
        printf("file_name is NULL\n");
    }else{
        printf("file_name is not NULL\n");
    }

    printf("저장된 파일 이름: %s\n", get_file_name());

    startDummyFoo(); // `uartdriver.h`에 정의된 함수 실행

    return 0;
}
