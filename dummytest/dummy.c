#include <stdio.h>
#include <string.h>  // memset 사용

int main() {
    // 4096 바이트 버퍼를 할당하고 0xFF로 초기화
    unsigned char buffer[4096];
    memset(buffer, 0xFF, sizeof(buffer));

    // 인덱스 0~3을 특정 값으로 오버라이딩 (예: 0x01, 0x02, 0x03, 0x04)
    buffer[0] = 0x01;
    buffer[1] = 0x02;
    buffer[2] = 0x03;
    buffer[3] = 0x04;

    // 앞부분(0~15바이트)만 출력하여 확인
    printf("Buffer (first 16 bytes):\n");
    for (int i = 0; i < 4096; i++) {
        printf("%02X ", buffer[i]);  // 16진수 형식으로 출력
    }
    printf("\n");

    return 0;
}
