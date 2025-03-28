 /*
  * author: datamining7830@gmail.com
  * Unit Test: Test를 위한  bin 생성
  * Purpose: 1~512 숫자로 bin 파일 생성
  * Description: 1~512 1바이트 단위 생성 -> ex) 15 -> 00001111
  */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h> // _getcwd 함수 사용을 위한 헤더 파일


// 리틀 엔디안 정수를 빅 엔디안 형식으로 변환하는 함수
unsigned int toBigEndian(unsigned int littleEndian) {
    unsigned int bigEndian = 0;
    bigEndian |= (littleEndian & 0x000000FF) << 24;
    bigEndian |= (littleEndian & 0x0000FF00) << 8;
    bigEndian |= (littleEndian & 0x00FF0000) >> 8;
    bigEndian |= (littleEndian & 0xFF000000) >> 24;
    return bigEndian;
}

int main(int argc, char *argv[]) {

    // printf("인수 개수: %d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("인수[%d]: %s\n", i, argv[i]);
    // }

    // char cwd[1024];
    // if (_getcwd(cwd, sizeof(cwd)) != NULL) {
    //     printf("현재 작업 디렉토리: %s\n", cwd);
    // } else {
    //     perror("현재 작업 디렉토리 확인 실패");
    // }
    FILE *pFile;
    int sectorSize = 1024;
    int sectorNum = 49;
    uint8_t numbers[196608];
    for (size_t i = 0; i < 196608; i++) {
        numbers[i] = (i)%256;
    }
    size_t numElements = sizeof(numbers) / sizeof(numbers[0]);
    printf("numElements: %zu\n", numElements);

    // 바이너리 모드로 파일 열기
    pFile = fopen("example.bin", "wb");
    if (pFile == NULL) {
        perror("파일 열기 실패");
        return 1;
    }

    // 배열을 빅 엔디안 형식으로 변환하여 바이너리 파일에 쓰기
    for (size_t i = 0; i < numElements; i++) {
       // unsigned int bigEndianNumber = toBigEndian(numbers[i]);
        size_t written = fwrite(&numbers[i], sizeof(uint8_t), 1, pFile);
        if (written != 1) {
            perror("파일 쓰기 실패");
            fclose(pFile);
            return 1;
        }
    }
    fseek(pFile, 0L, SEEK_END);
    int sz=ftell(pFile);
    printf("파일 크기: %d 바이트\n", sz);

    fclose(pFile);
    return 0;
}