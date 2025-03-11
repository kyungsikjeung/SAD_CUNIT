#include <stdio.h>
#include <string.h>

static char file_name[256];


void setFileName(const char* name){
    strcpy(file_name, name);
}

const char* get_file_name(){
    return file_name;
}

int main(){
    printf("Hello from test4\n");
    // 파일 이름을 입력 받아서 저장
    printf("파일 이름을 입력하세요: ");
    scanf("%s", file_name);
    printf("입력한 파일 이름: %s\n", file_name);
    printf("Setter 파일 이름: %s\n", "test4.txt");
    setFileName("test4.txt");
    printf("저장된 파일 이름: %s\n", get_file_name());

    return 0;
}