#include "spi_bypass_driver.h"


/* Bypass SPI Init */
void SPI_BypassDriver_Init(){
    GPIO_OutputBypass(SPI_EIRQ, 1u); // HW Enable 
	registerBypassSetting(); // GMSL 레지스터 설정
    TIMER_DelayMsec(10); 
    DisableIRQ(PORTC_IRQn); // Ramres to RO
    gpio_pin_config_t outputConfig = {
		kGPIO_DigitalOutput, 1,
	};
    // RO 출력 설정
	PORT_SetPinMux(RO_PORT, RO_PIN, kPORT_MuxAsGpio);
    GPIO_PinInit(RO_GPIO, RO_PIN, &outputConfig); 
    gpio_pin_config_t inputConfig = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0
    };
    // BNE 입력 설정
    PORT_SetPinMux(PORTC, BNE_PIN, kPORT_MuxAsGpio);
    PORTC->PCR[BNE_PIN] = ((PORTC->PCR[BNE_PIN] &
                      (~(PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))
                     | (uint32_t)(PORT_PCR_PE_MASK));
    GPIO_PinInit(BNE_GPIO, BNE_PIN, &inputConfig);
}

/* Bypass SPI DE Init */
void SPI_BypassDriver_DeInit(){
    GPIO_InputPin_Init(); //RO -> Ramres 전환
    PORT_SetPinMux(BNE_PORT, BNE_PIN, kPORT_MuxAlt2); // BNE 를 제어기 SPI를 위해 변경
    GPIO_OutputBypass(SPI_EIRQ, 0u); 
}

/*  High - 명령모드 Low - 데이터 전송    */
void SPI_BypassDriver_SetRo(uint8_t value){
    GPIO_WritePinOutput(RO_GPIO,RO_PIN,value); // SET Command Mode (RO = 1)
}

/* FIFO 처리 데이터 유 - HIGH, 없을 때 LOW (Vice Versa)  */
bool SPI_BypassDriver_HasReadData(){
    while(!GPIO_ReadPinInput(BNE_GPIO,BNE_PIN) ){
    }
    return true;
}


bool SPI_BypassDriver_waitBufferEmpty(){
    // while(SPI_BypassDriver_HasReadData()) <- 속도 저하
    while(GPIO_ReadPinInput(BNE_GPIO,BNE_PIN) ){
    }
    return true;
}

/* Single */
void SPI_BypassDriver_ByteWrite(uint8_t data) {
    while (!(SPI0->SR & kDSPI_TxFifoFillRequestFlag)) { } 
    SPI0->PUSHR = SPI_PUSHR_CTAS(kDSPI_Ctar0) |
                  SPI_PUSHR_PCS(kDSPI_Pcs1) |
                  SPI_PUSHR_EOQ(false) |
                  SPI_PUSHR_CTCNT(false) |
                  SPI_PUSHR_CONT(true) |
                  SPI_PUSHR_TXDATA(data);  

    SPI0->MCR &= ~SPI_MCR_HALT_MASK; // Halt 비트 반전 
    while (!(SPI0->SR & kDSPI_TxCompleteFlag)) {} // 전송 후 Tx Complete Flag 확인
    SPI0->SR |= SPI_SR_TCF_MASK;
    SPI0->MCR |= SPI_MCR_CLR_TXF_MASK; 
    SPI0->MCR |= SPI_MCR_CLR_RXF_MASK; 
}

/*
*  SS1 Assert 이후 디바이스 사용될 명령 전송
*  @param data : 전송할 데이터(Dev Command + Addr)
* @param length : 전송할 데이터 길이
*
*/
void SPI_BypassDriver_SendCommand(uint8_t *data, uint8_t length){
    SPI_BypassDriver_SetRo(1); // 1. Set RO
    SPI_BypassDriver_waitBufferEmpty(); // 2. Check BNE to ensure that the buffer is empty(Optional)
    SPI_BypassDriver_ByteWrite(0xA4); // 3. Send 0xA4/A5 (Assert SS1/SS2).
    SPI_BypassDriver_SetRo(0); // 4. Clear RO
    for(int i=0; i<length;i++ ){  // 5~6. Send Cmd Byte || Send Write Byte
        SPI_BypassDriver_ByteWrite(data[i]); 
    }
}

void SPI_BypassDriver_SendCommandWithoutClock(uint8_t *data, uint8_t length){
    SPI_BypassDriver_SetRo(1); // 1. Set RO
    SPI_BypassDriver_ByteWrite(0xA4); 
    SPI_BypassDriver_SetRo(0); // 4. Clear RO
    for(int i=0; i<length;i++ ){  // 5~6. Send Cmd Byte || Send Write Byte
        SPI_BypassDriver_ByteWrite(data[i]); 
    }
}

/* SW Writing Protection(SPI) -> HW Writing Protection은 SPI INIT에 이미 설정함 */
void SPI_BypassDriver_FlashWriteEnable(bool enable){
    SPI_OperationCode operationCode = enable ? FLASH_WRITE_ENABLE:FLASH_WRITE_DISABLE;
    uint8_t commandData = (uint8_t)operationCode; 
    SPI_BypassDriver_SendCommand(&commandData,1); 
    SPI_BypassDriver_SetRo(1);  //  Command Mode , GPIO HIGH
    SPI_BypassDriver_HasReadData(); // wait Data
    SPI_BypassDriver_ByteWrite(0xA6); // Disconnection
    SPI_BypassDriver_SetRo(0); // Finish Enable/Disable Flash Driver
}

void SPI_BypassDriver_SectorWrite(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data){
    SPI_BypassDriver_Init(); //      
    uint16_t startSector  = page / PAGES_IN_SECTOR; 
	uint16_t endSector  = (page + ((size+offset-1)/PAGE_SIZE)) / PAGES_IN_SECTOR; 
	uint16_t numSectors = endSector-startSector+1; 

	uint32_t sectorOffset = ((page% PAGES_IN_SECTOR )*PAGE_SIZE)+offset; // 페이지로 섹터에 쓰여질 오프셋 메모리 주소 계산
	uint32_t dataindx = 0;
    uint8_t dataToWrite[SECTOR_SIZE] = {0xFF}; // 섹터당 쓸 데이터 수
	for (uint16_t i=0; i<numSectors; i++){ // 섹터별 데이터 쓰기
        memset(dataToWrite, 0xFF, SECTOR_SIZE);
        uint32_t startPage = startSector*PAGES_IN_SECTOR; // 섹터 0 - 0페이지 , 섹터 1 - 16페이지
        uint16_t bytesRemaining = (size+sectorOffset)<SECTOR_SIZE ? size : SECTOR_SIZE - sectorOffset;
        for (uint16_t i=0; i<bytesRemaining; i++)
		{
            if(dataindx + i >= size){
                dataToWrite[i+sectorOffset] = 0xFF;
            }else{
			    dataToWrite[i+sectorOffset] = data[i+dataindx];
            }
            
		}
        //  Sector Write & Erase

        SPI_BypassDriver_PageWriteAndSectorClean(startPage, 0, SECTOR_SIZE, dataToWrite);
        startSector++;
		sectorOffset = 0;
		dataindx = dataindx+bytesRemaining; // 데이터 index 추가
		size = size-bytesRemaining; // 데이터 처리한 만큼 사이즈 차감
    }
    SPI_BypassDriver_FlashWriteEnable(false); 
    SPI_BypassDriver_DeInit();
}



// ! 파형 찎어볼것 
void SPI_BypassDriver_BurstWrite(uint8_t *commandData, uint8_t commandLength, uint8_t* data, uint32_t length) {
    
    SPI_BypassDriver_SendCommand(commandData,commandLength); 
   // SPI_BypassDriver_SendCommandWithoutClock(commandData,commandLength); // Step 1-6
    for(uint32_t i=0; i< length; i++  ){ // 12. Repeat (7-11) 
        PRINTF("Data %d th: %d\r\n",i,data[i]);
        SPI_BypassDriver_ByteWrite(data[i]); // 7. Send Write Byte.
        SPI_BypassDriver_SetRo(1); // 8. Set RO.
        SPI_BypassDriver_HasReadData(); // 9. Wait for BNE = 1.
        SPI_BypassDriver_ByteWrite(0x00); // 10. Send Write Byte./ Read Byte(Discard)
        SPI_BypassDriver_SetRo(0); // 11. Clear RO.
    }
    SPI_BypassDriver_SetRo(1); // 13. Set RO
    SPI_BypassDriver_HasReadData(); // 14. Wait for BNE = 1.
    SPI_BypassDriver_ByteWrite(0xA6); //15. Send 0xA6/Read Byte (Discard) (Clear SS).
    SPI_BypassDriver_ByteWrite(0xA6); //16. Send 0xA6/Read Byte (Discard) (Clear SS).
    SPI_BypassDriver_ByteWrite(0xA6); //16. Send 0xA6/Read Byte (Discard) (Clear SS).
    SPI_BypassDriver_ByteWrite(0xA6); //16. Send 0xA6/Read Byte (Dis(Clear SS).
}


void testtest(){
    // SPI_BypassDriver_FlashWriteEnable(true); // Write Enable S/W
    SPI_BypassDriver_BurstWriteTest();
    SPI_BypassDriver_BurstWriteTest();
}




// ! Test , Command 
void SPI_BypassDriver_BurstWriteTest() {
    SPI_BypassDriver_Init(); // GPIO 설정, Ramres OUTPUT 변경 , BNE INPUT 설정
	// SPI_BypassDriver_FlashWriteEnable(true); // Write Enable S/W
    uint8_t commandData[4] = {FLASH_PAGE_WRITE, 0x00, 0x00,0x00}; // ! Page Write + 3Byte Addr
    uint8_t commandLength = 4;  // 5
    // Data Start 

    SPI_BypassDriver_SendCommandWithoutClock(commandData,commandLength); // ! Command
    uint8_t data[255];
    uint8_t length = 255;
    for(int i=0; i<255;i++){
        data[i] = i;
    }

    for(uint32_t i=0; i< length; i++  ){ // 12. Repeat (7-11) 
        SPI_BypassDriver_ByteWrite(data[i]); // 7. Send Write Byte.
        SPI_BypassDriver_SetRo(1); // 8. Set RO.
        SPI_BypassDriver_HasReadData(); // 9. Wait for BNE = 1.
        SPI_BypassDriver_ByteWrite(0x00); // 10. Send Write Byte./ Read Byte(Discard)
        SPI_BypassDriver_SetRo(0); // 11. Clear RO.
    }
    SPI_BypassDriver_SetRo(1); // 13. Set RO
    SPI_BypassDriver_HasReadData(); // 14. Wait for BNE = 1.
    SPI_BypassDriver_ByteWrite(0xA6); //15. Send 0xA6/Read Byte (Discard) (Clear SS).
    SPI_BypassDriver_ByteWrite(0xA6); //16. Send 0xA6/Read Byte (Discard) (Clear SS).
    SPI_BypassDriver_ByteWrite(0xA6); //15. Send 0xA6/Read Byte (Discard) (Clear SS).
    // SPI_BypassDriver_ByteWrite(0xA6); //16. Send 0xA6/Read Byte (Discard) (Clear SS).
    // Data End
    TIMER_DelayMsec(100);
    PRINTF("EN1D\r\n");
    // SPI_BypassDriver_FlashWriteEnable(false);
    PRINTF("END\r\n");
    SPI_BypassDriver_DeInit();
}

// ! Burst Read Test
void SPI_BypassDriver_BurstReadTest(uint8_t *rData, uint16_t rDataLength){
	SPI_BypassDriver_Init();
    // ! mockup command + Addr
    uint8_t command[5] = {FLASH_FAST_READ,0x00,0x00,0x00,0x00};
    uint8_t commandLength = sizeof(command) / sizeof(command[0]);  // 5
	//SPI_BypassDriver_SendCommandWithoutClock(command,commandLength); // ! Step 1-5
    SPI_BypassDriver_SendCommand(command,commandLength); // ! Step 1-5
    SPI_BypassDriver_SetRo(1); // ! Step 6  
    uint8_t dummy = 0x00; // dummy 에 데이터 읽을 시 의미 없는 값 저장    
    // ! Step 7 . 8번과정 (OP + ADDR1~3  값을 제거 해주는것이 필요)
    SPI_BypassDriver_HasReadData(); // OP CODE 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    SPI_BypassDriver_HasReadData(); // ADDR1 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    SPI_BypassDriver_HasReadData(); // ADDR2 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    SPI_BypassDriver_HasReadData(); // ADDR3 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    //   FAST READ DUMMY 8 CLK 제거
    SPI_BypassDriver_HasReadData(); // Dummy 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    // ! Step 7.8 과정 완료
    for(uint32_t i=0; i<rDataLength; i++){ // Send Data
        SPI_BypassDriver_HasReadData();
        SPI_BypassDriver_readData(0xA7,&rData[i]);
        PRINTF("READDATA%d\r\n",rData[i]);
    }
    SPI_BypassDriver_HasReadData();
    SPI_BypassDriver_ByteWrite(0xA6); // Clear SS
    SPI_BypassDriver_ByteWrite(0xA6); // Clear SS
//    SPI_BypassDriver_ByteWrite(0xA6); // Clear SS
//    SPI_BypassDriver_ByteWrite(0xA6); // Clear SS
    SPI_BypassDriver_DeInit();
}


/* Read Command - 0xA7 - discard two bytes, 0xFF 데이터 읽기 */
void SPI_BypassDriver_readData(uint8_t readCommand,uint8_t* data){
    while (!(SPI0->SR & kDSPI_TxFifoFillRequestFlag)) { } /* TX FIFO empty 일 때까지 대기 */
        SPI0->PUSHR = SPI_PUSHR_CTAS(kDSPI_Ctar0) | // CS 설정
                    SPI_PUSHR_PCS(kDSPI_Pcs1) |
                    SPI_PUSHR_EOQ(true) |
                    SPI_PUSHR_CTCNT(false) |
                    SPI_PUSHR_CONT(true) |
                    SPI_PUSHR_TXDATA(readCommand); // uint16_t

        SPI0->MCR &= ~SPI_MCR_HALT_MASK; // 전송 시작
        while (!(SPI0->SR & kDSPI_TxCompleteFlag)) {} // 상태 완료로 변경

        // 전송 완료 플래그 클리어
        SPI0->SR |= SPI_SR_TCF_MASK;
        SPI0->MCR |= SPI_MCR_CLR_TXF_MASK;
        *data = DSPI_ReadData(SPI0);
        SPI0->MCR |= SPI_MCR_CLR_RXF_MASK;
}
// SPI_BypassDriver_BurstRead()
void SPI_BypassDriver_BurstRead(uint8_t *command, uint8_t commandLength, uint8_t *rData, uint32_t rDataLength){
	SPI_BypassDriver_Init(); // ! 시작
	SPI_BypassDriver_SendCommand(command,commandLength);
    SPI_BypassDriver_SetRo(1); // Read mode   
    uint8_t dummy = 0x00; // dummy 에 데이터 읽을 시 의미 없는 값 저장    
    SPI_BypassDriver_HasReadData(); // OP CODE 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    SPI_BypassDriver_HasReadData(); // ADDR1 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    SPI_BypassDriver_HasReadData(); // ADDR2 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    SPI_BypassDriver_HasReadData(); // ADDR3 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    //   FAST READ DUMMY 8 CLK 제거
    SPI_BypassDriver_HasReadData(); // Dummy 제거
    SPI_BypassDriver_readData(0xA7,&dummy);
    for(uint32_t i=0; i<rDataLength; i++){ // Send Data
        SPI_BypassDriver_HasReadData();
        SPI_BypassDriver_readData(0xA7,&rData[i]);
    }
    SPI_BypassDriver_HasReadData();
    SPI_BypassDriver_ByteWrite(0xA6); // Clear SS
    SPI_BypassDriver_ByteWrite(0xA6); // Clear SS
    SPI_BypassDriver_DeInit(); // ! 끝
}

void testBurstRead(){
    //SPI_BypassDriver_SetPath(CONNECT_FLASH_REGISTER_VALUE); // Path 설정
    SPI_BypassDriver_Init(); // SPI I2C 세팅
    uint8_t commandData[5] = {0x0B, 0x01, 0x02,0x03,0x00};
    uint8_t rData[1024] = {0};
    SPI_BypassDriver_BurstRead(commandData,5,rData,1024);
    
    for(int i=0; i<10;i++){
        PRINTF("READ Data %d th: %d\r\n",i,rData[i]);
    }
    SPI_BypassDriver_DeInit();
}

/* Flash Logic */
/* Flash ID Read */
void SPI_BypassDriver_FlashReadID(){
    SPI_BypassDriver_Init();
	uint8_t rData[4] = {0};
    uint8_t commandData[4] = {0x9F, 0x00, 0x00,0x00}; // Command + dummy 3byte to read ID(0x00)
    SPI_BypassDriver_SendCommand(commandData,4);
    SPI_BypassDriver_SetRo(1); 
    SPI_BypassDriver_HasReadData();
    SPI_BypassDriver_ByteWrite(0xA6); // Discard 0x9F
    SPI_BypassDriver_HasReadData();
    for(uint8_t i =0 ;  i<3; i++){
        SPI_BypassDriver_readData(0xFF,&rData[i]);
    }  
    SPI_BypassDriver_SetRo(0); // 데이터 전송 완료
    PRINTF("Flash Read Result\r\n");
    for(int i=0; i<3;i++){
    	//PRINTF("READ Data %02X th: \t",i,rData[i]);

    	PRINTF("%02X\t", rData[i]);
    }
    PRINTF("\r\n");
    SPI_BypassDriver_DeInit();
}

void SPI_BypassDriver_FlashSectorErase(uint16_t numsector){ 
    SPI_OperationCode eraseSectorOpCode = FLASH_ERASE_SECTOR ;
	uint32_t memAddr = numsector*SECTOR_SIZE;  
    uint8_t commandData[4];
    commandData[0] = (uint8_t)eraseSectorOpCode; // 섹터 제거 OP CODE
    commandData[1] = (memAddr>>16)&0xFF;  // MSB of the memory Address
    commandData[2] = (memAddr>>8)&0xFF;
    commandData[3] = (memAddr)&0xFF; // LSB of the memory Address
    uint8_t commandDataLength = sizeof(commandData) / sizeof(commandData[0]);  

    SPI_BypassDriver_SendCommandWithoutClock(commandData,commandDataLength); // Sector Erase Command + Addr
    SPI_BypassDriver_SetRo(1); 
    SPI_BypassDriver_HasReadData();
    for(uint8_t i=0; i<commandDataLength;i++){
        uint8_t command = i == 0 ? 0xA6 : 0xFF; // Deassert SS1 + prevent buffer not empty(remvoe data)
        SPI_BypassDriver_HasReadData(); // Wait Buffer is not empty
        SPI_BypassDriver_ByteWrite(command);
    }   
    SPI_BypassDriver_SetRo(0); // 데이터 전송 완료
    TIMER_DelayMsec(SECTOR_ERASE_DELAY);
}


/* 페이지 라이트 */
void SPI_BypassDriver_PageWrite(){
    SPI_BypassDriver_Init();
    SPI_BypassDriver_DeInit();

}


void SPI_BypassDriver_PageWriteAndSectorClean(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data){
    uint32_t startPage = page; // 시작 페이지
    uint32_t endPage  = startPage + ((size+offset-1)/PAGE_SIZE);
    uint32_t numPages = endPage - startPage + 1;
    uint16_t startSector  = startPage / PAGES_IN_SECTOR;
    uint16_t endSector  = endPage / PAGES_IN_SECTOR;
    uint16_t numSectors = endSector - startSector + 1;
    uint8_t tData[PAGE_SIZE] =  {0xFF}; // 안쓰는 값 FF로 초기화
   
    /* 섹터 Erase 
    ! TODO Block Erase 로 여기 부분 지울것, 상위 호출단에서 제어 해야함.
    */
    for (uint16_t i = 0; i < numSectors; i++)
    {
        SPI_BypassDriver_FlashWriteEnable(true); 
        SPI_BypassDriver_FlashSectorErase(startSector++);
    }

    uint32_t dataPosition = 0; 
    
    /* Pages Writing */
    for (uint32_t i = 0; i < numPages; i++)
    {
        PRINTF("num page %d\r\n",i);
        const SPI_OperationCode pageWriteOpCode = FLASH_PAGE_WRITE;
        uint32_t memAddr = (startPage * PAGE_SIZE) + offset;
        uint16_t bytesremaining  = (size > (PAGE_SIZE - offset)) ? (PAGE_SIZE - offset) : size;  // 단일 페이지 쓰기 가능한 데이터 양, 페이지 쓸 때마다 계산
        uint8_t commandData[4];
        uint32_t indx = 4;
        commandData[0] = (uint8_t)pageWriteOpCode;  // page program
        commandData[1] = (memAddr >> 16) & 0xFF;  // MSB of the memory Address
        commandData[2] = (memAddr >> 8) & 0xFF;
        commandData[3] = (memAddr) & 0xFF; // LSB of the memory Address
        // SPI_BypassDriver_FlashWriteEnable(false);

        memset(tData, 0xFF, PAGE_SIZE);
        for (uint32_t j = 0; j < bytesremaining; j++)
        {
            //tData[j] = data[j + dataPosition];  // SPI 통신 버퍼에  OP코드 + 주소 + 데이터 저장
            if (j + dataPosition < size) {
                tData[j] = data[j + dataPosition]; // SPI 통신 버퍼에 OP코드 + 주소 + 데이터 저장
            } else {
                tData[j] = 0xFF; // 남은 부분을 0xFF로 채움
            }

        }
        SPI_BypassDriver_FlashWriteEnable(true); 
        SPI_BypassDriver_BurstWrite(commandData,4,tData,bytesremaining); // 페이지 라이팅 종료
        startPage++; // 다음 페이지로 이동
        offset = 0; //  offset 초기화
        size -= bytesremaining; // 처리한 데이터 크기 제거
        dataPosition += bytesremaining; // 쓴 데이터 만큼 데이터 포인터 이동
        TIMER_DelayMsec(NEXT_PAGE_WRITING_DELAY);
    }
}



typedef enum {
    BLOCK,
    SECTOR
}ERASE_TYPE;




void Erase(unsigned long fileSize){
   
    const unsigned long BLOCK_SIZE =  65536;
    unsigned long chunkBlockNum  = fileSize / BLOCK_SIZE;  // 지울 블럭 갯수
    unsigned long chunkSectorNum = (fileSize - (BLOCK_SIZE * chunkBlockNum)) // 지울 섹터 갯수
    bool isRemainder = (fileSize % SECTOR_SIZE) != 0; // 섹터 나머지 존재 여부
    if(isRemainder){
        chunkSectorNum += 1; // 나머지가 있으면 지울 섹터 하나 더 추가
    }
    uint32_t startAddr = 0; // 시작주소
    for(unsigned long i=0; i<chunkBlockNum; i++){
        EraseByAddress(BLOCK, startAddr);
        startAddr += BLOCK_SIZE;
    }
    for(unsigned long j=0 ; j<chunkSectorNum; j++){
        EraseByAddress(SECTOR, startAddr);
        startAddr += SECTOR_SIZE;
    }
    


}
