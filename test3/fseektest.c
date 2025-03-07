#include <stdio.h>
#include <stdlib.h>
#include <direct.h> // _getcwd 함수 사용을 위한 헤더 파일



//

// 리틀 엔디안 정수를 빅 엔디안 형식으로 변환하는 함수
unsigned int toBigEndian(unsigned int littleEndian) {
    unsigned int bigEndian = 0;
    bigEndian |= (littleEndian & 0x000000FF) << 24;
    bigEndian |= (littleEndian & 0x0000FF00) << 8;
    bigEndian |= (littleEndian & 0x00FF0000) >> 8;
    bigEndian |= (littleEndian & 0xFF000000) >> 24;
    return bigEndian;
}

int main() {

    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("현재 작업 디렉토리: %s\n", cwd);
    } else {
        perror("현재 작업 디렉토리 확인 실패");
    }


    FILE *pFile;
    int numbers[] = {1, 2, 3, 4, 5};
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
        unsigned int bigEndianNumber = toBigEndian(numbers[i]);
        size_t written = fwrite(&bigEndianNumber, sizeof(int), 1, pFile);
        if (written != 1) {
            perror("파일 쓰기 실패");
            fclose(pFile);
            return 1;
        }
    }

    fclose(pFile);
    return 0;
}