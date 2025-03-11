#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024  // 1KB 버퍼

int main() {
    char filename[256];  // 파일 이름 저장 변수
    FILE *file;
    char buffer[BUFFER_SIZE];  // 파일을 읽을 버퍼
    size_t bytesRead;

    // 사용자로부터 파일명 입력 받기
    printf("읽을 파일명을 입력하세요: ");
    scanf("%255s", filename);  // 최대 255자까지 입력

    // 파일 열기 (바이너리 읽기 모드 "rb")
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("파일을 열 수 없습니다");
        return 1; // 오류 종료
    }

    printf("\n[파일 내용]\n");

    // 파일을 1KB 단위로 읽어서 출력
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        fwrite(buffer, 1, bytesRead, stdout);  // 읽은 내용을 출력
    }

    // 파일 닫기
    fclose(file);

    return 0;
}
