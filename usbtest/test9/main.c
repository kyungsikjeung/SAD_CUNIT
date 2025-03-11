#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


/*
 * author: datamining7830@gmail.com
 * Unit Test: 
 * Input : FileSize
 * Output : Erase Type 
 * Purpose: File Size에 따라 블럭 Erase 와 섹터 Erase 수행하는 함수 Test
 * Description: USB로 파일을 입력을 받음. 파일을 입력 받아 파일 사이즈를 계산하고, 
 * 파일 사이즈에 따라 블럭 Erase와 섹터 Erase를 수행하는 함수 Test
 * 
 */
typedef unsigned long DWORD;

#define BLOCK_SIZE (10U)
#define SECTOR_SIZE (5)
#define PAGE_SIZE (3)


#define SETOR_ERASE_OP 0x01
#define BLOCK_ERASE_OP 0x02
#define PAGE_WRITE_OP 0x03



typedef enum {
    BLOCK,
    SECTOR
}ERASE_TYPE;


void EraseByAddress(ERASE_TYPE type, uint32_t adress){
    uint8_t tData[4];
    memset(tData,0, sizeof(tData));
    tData[0] = (type == BLOCK) ? BLOCK_ERASE_OP : SETOR_ERASE_OP;
    tData[1] = (adress >> 16) & 0xFF;  // MSB of the memory Address
    tData[2] = (adress >> 8) & 0xFF;
    tData[3] = (adress) & 0xFF; // LSB of the memory Address
    // Do something .... GMSL 
    // 실제로 지울 로직에 대해서 작성 
    printf("Erase Type: %d, Address: %d\n", tData[0], (tData[1] << 16) | (tData[2] << 8) | tData[3]);
}


// Erase Function
void Erase(unsigned long fileSize){
    DWORD chunkBlockNum  = fileSize / BLOCK_SIZE; 
    DWORD chunkSectorNum = (fileSize - (BLOCK_SIZE * chunkBlockNum))  / SECTOR_SIZE + 1;
    assert(chunkBlockNum == 2);
    uint32_t startAddr = 0;
    
    for(int i=0; i<chunkBlockNum; i++){
        EraseByAddress(BLOCK, startAddr);
        startAddr += BLOCK_SIZE;
    }
    for(int j=0 ; j<chunkSectorNum; j++){
        EraseByAddress(SECTOR, startAddr);
        startAddr += SECTOR_SIZE;
    }
}


void setDummyData(uint8_t* data, int size){
    for(int i=0; i<size; i++){
        data[i] = i;
    }
}






void SPI_PAGE_WRTIE(uint32_t addr, uint8_t size, uint8_t* data){
    uint8_t tData[PAGE_SIZE+4]; 
    tData[0] = (uint8_t)PAGE_WRITE_OP; // Page Write OP
    tData[1] = (addr >> 16) & 0xFF;  // MSB of the memory Address
    tData[2] = (addr >> 8) & 0xFF;
    tData[3] = (addr) & 0xFF; // LSB of the memory Address
    memcpy(&tData[4], data, PAGE_SIZE);
    for(int i=0; i< PAGE_SIZE+4; i++){
        printf("0x%d\t ", tData[i]);
    }
    printf("\r\npage Write Done\r\n");
} 


// Host msd. c 
int main(){
    unsigned long file_size = 46;
    uint8_t fileData[file_size];
    memset(fileData, 0, sizeof(fileData));
    Erase(file_size); // Block & Sector Erase Test
    setDummyData(fileData, file_size); // Create Dummy Data for SPI ....
    DWORD fileChunkNum = (file_size/ PAGE_SIZE)+1;
    unsigned long  byteRemaining = file_size;
    uint32_t startAddr = 0;
    for(DWORD i=0; i<fileChunkNum; i++){
        if(byteRemaining < PAGE_SIZE){
            SPI_PAGE_WRTIE(startAddr, byteRemaining, fileData);
            break;
        }else{
            SPI_PAGE_WRTIE(startAddr,PAGE_SIZE, fileData);
        }
        startAddr += PAGE_SIZE;
        byteRemaining -= PAGE_SIZE;
    }

    return 0;
}

