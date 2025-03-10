#include <stdio.h>

static char file_name[256];

int main(){
    printf("Hello from test4\n");
    // 파일 이름을 입력 받아서 저장
    printf("파일 이름을 입력하세요: ");
    scanf("%s", file_name);
    printf("입력한 파일 이름: %s\n", file_name);


    return 0;
}