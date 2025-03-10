# UART 드라이버 단위 테스트

## 설명
유닛 테스트 UART 드라이버에서 받은 파일 이름이 올바르게 저장되고 검색되는지 검증 . 프로그램은 명령행 인자로 파일 이름을 받아 `setFileName()`을 사용하여 설정한 후, `get_file_name()`을 통해 파일 이름이 올바르게 저장되었는지 확인함.

## 기능
- 명령행 인자로 파일 이름을 입력받음 ( ./test5 abc.txt )
- 파일 이름을 배열에 저장 (abc.txt)
- 설정 전후로 파일 이름이 비어 있는지 확인


## 컴파일 및 실행 방법
### 프로그램 컴파일:
```sh
gcc -o test5 test5.c uartdriver.c usb.c
```

### 파일 이름과 함께 프로그램 실행:
```sh
./test5 test123.txt
```

## 예상 출력 결과
프로그램이 정상적으로 실행되면 다음과 같은 출력이 나타납니다:
```sh
Hello from test5
file_name is NULL
입력한 파일 이름: test123.txt
file_name is not NULL
저장된 파일 이름: test123.txt
```

## 함수 설명
### `void setFileName(const char* name)`
입력받은 파일 이름을 정적 문자 배열에 저장합니다.

### `const char* get_file_name()`
저장된 파일 이름을 반환합니다.

### `int main(int argc, char* argv[])`
1. 프로그램 시작 시 `file_name`이 비어 있는지 확인
2. 파일 이름 인자가 제공되었는지 확인
3. 제공된 파일 이름을 저장
4. `file_name`이 올바르게 업데이트되었는지 확인
5. `startDummyFoo()` 호출

## 오류 처리
-  파일 이름을 입력하지 않으면 오류 메시지를 출력하고 종료 (./test5 test123.txt)


