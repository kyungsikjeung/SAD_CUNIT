## USB TEST

# STEP 1
Bin 파일 생성
1~512 Binary 파일 생성 프로그램 작성

# STEP 2
총 File 사이즈에서 256 바이트씩 청크함. 

# STEP 3
총 File 사이즈에서 256 바이트씩 리드 하면서 , CRC 계산함.

# STEP 4
외부 파일에서 파일명 Getter , Setter 함수 테스트 함. 

# STEP 5
USB 파일이 인식 되면, Init 함수가 작동하면서 하드코딩된 파일 명 읽혀짐. 
이를 방지하고자, Getter , Setter 함수를 추가하고 , 파일명 입력 없을 시, 확인 하는 로직 유닛 테스트함

