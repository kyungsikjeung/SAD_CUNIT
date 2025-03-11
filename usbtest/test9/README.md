# 파일 지우기 및 페이지 쓰기 유닛 테스트

## 작성자
datamining7830@gmail.com

## 유닛 테스트
Flash Page Write 를 테스트 하기 위함.

## 목적
USB 파일에서 Chunk 된 데이터를 파일 사이즈 만큼 블럭 혹은 섹터 단위로 Erase 후
Page 단위 만큼 SPI 데이터 전송하기 위함.

## 설명
프로그램은 파일 크기를 입력으로 받아 파일 크기에 따라 블록 및 섹터 지우기 작업을 수행합니다. 또한, 파일 크기에 따라 페이지 단위로 데이터를 쓰는 작업을 수행합니다. 프로그램은 각 작업에 대해 지우기 유형과 주소, 페이지 쓰기 데이터를 출력합니다.

## 사용법
1. 프로그램을 컴파일합니다:
    ```sh
    gcc -g main.c -o main.exe
    ```

2. 프로그램을 실행합니다:
    ```sh
    ./main.exe
    ```

## 코드 설명

### 정의
- `BLOCK_SIZE`: 블록의 크기 (10 바이트).
- `SECTOR_SIZE`: 섹터의 크기 (5 바이트).
- `PAGE_SIZE`: 페이지의 크기 (3 바이트).
- `SETOR_ERASE_OP`: 섹터 지우기 작업 코드.
- `BLOCK_ERASE_OP`: 블록 지우기 작업 코드.
- `PAGE_WRITE_OP`: 페이지 쓰기 작업 코드.

### 열거형
- `ERASE_TYPE`: 지우기 유형 (BLOCK 또는 SECTOR).

### 함수
- `void EraseByAddress(ERASE_TYPE type, uint32_t address)`: 이 함수는 지우기 유형과 주소에 따라 지우기 작업을 수행합니다. 지우기 유형과 주소를 출력합니다.
- `void Erase(unsigned long fileSize)`: 이 함수는 파일 크기에 따라 블록 및 섹터의 수를 계산하고 지우기 작업을 수행합니다.
- `void setDummyData(uint8_t* data, int size)`: 이 함수는 더미 데이터를 설정합니다.
- `void SPI_PAGE_WRTIE(uint32_t addr, uint8_t size, uint8_t* data)`: 이 함수는 페이지 단위로 데이터를 씁니다. 페이지 쓰기 데이터를 출력합니다.

### 메인 함수
`main` 함수는 파일 크기를 46 바이트로 초기화하고 `Erase` 함수를 호출하여 지우기 작업을 수행합니다. 그런 다음, 더미 데이터를 생성하고 페이지 단위로 데이터를 씁니다.

