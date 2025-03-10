/*
 * author: datamining7830@gmail.com
 * Unit Test: CRC32
 * Purpose: aaa.bin 파일의 CRC 32 계산
 * Description: 
 * 1. CRC32 계산
 * 2. CRC32 계산 결과 출력 : aaa.bin 파일 계산 결과 70083813
 * 
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
 
int main() { 
    FILE *binFile = fopen("C:\\Users\\MD_10557\\Desktop\\jks\\test_c\\usbtest\\test3\\aaa.bin", "rb");    
    
    if (binFile == NULL) { 
        perror("Error opening binary file"); 
        return EXIT_FAILURE; 
    } 

    unsigned char buffer[256]; 
    size_t bytesRead; 

    uint32_t crc = 0xFFFFFFFF; // CRCSEED: 초기 값
    uint32_t poly = 0x04C11DB7; // CRCPOLY: CRC32 다항식
    uint32_t xor_out = 0xFFFFFFFF; // CRCFXOR: 출력 XOR 값
    int chunkNum = 0;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), binFile)) > 0) { 
        chunkNum++;
        printf("chunkNum: %lu bytesRead: %zu\n", chunkNum ,bytesRead);
        for (size_t i = 0; i < bytesRead; ++i) { 
            
            crc ^= (buffer[i] << 24);  // 바이트 처리
            for (int j = 0; j < 8; j++) {  // 비트 처리
                if (crc & 0x80000000) { // 최상의 비트가 1이라면
                    crc = (crc << 1) ^ poly;
                } else { // 아니라면 시프트 
                    crc <<= 1;
                }
            }
        }  
    } 
    printf("%08X", crc ^ xor_out);
    fclose(binFile); 
  
    return EXIT_SUCCESS; 
} 