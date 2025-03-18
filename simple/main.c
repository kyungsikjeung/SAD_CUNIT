#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

void strtolTest(){
    char a[10];  
    memset(a, '\0', sizeof(a));   
    strncpy(a, "12ABCD", sizeof(a) - 1); // 안전한 문자열 복사

    uint32_t u32_addr = strtol(a, NULL, 16) & 0xFFFFFF; // 24비트 초과 방지
    uint8_t u8_addr[3] = {0};

    u8_addr[0] = (u32_addr >> 16) & 0xFF;
    u8_addr[1] = (u32_addr >> 8) & 0xFF;
    u8_addr[2] = (u32_addr) & 0xFF;

    // 오류 감지
    if (u8_addr[0] != 0x00 || u8_addr[1] != 0x00 || u8_addr[2] != 0x00) {
        printf("Error: Address conversion failed!\n");
    }

    assert(u8_addr[0] == 0x32);
    assert(u8_addr[1] == 0xAB);
    assert(u8_addr[2] == 0xCD);

    printf("Test success: %06X\n", u32_addr);
}

int main() {
    strtolTest();
    return 0;
}
