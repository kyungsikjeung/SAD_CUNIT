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


#define SETOR_ERASE_OP 0x01
#define BLOCK_ERASE_OP 0x02


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
    // print tData
    printf("Erase Type: %d, Address: %d\n", tData[0], (tData[1] << 16) | (tData[2] << 8) | tData[3]);
}



void Erase(unsigned long fileSize){
    DWORD chunkBlockNum  = fileSize / BLOCK_SIZE; 
    assert(chunkBlockNum ==4);
    bool isRemainder = (fileSize % SECTOR_SIZE) != 0;
    assert(isRemainder == false);
    DWORD chunkSectorNum = (fileSize - (BLOCK_SIZE * chunkBlockNum))  / SECTOR_SIZE;
    
    if(isRemainder){
        chunkSectorNum += 1;
    }

    uint32_t startAddr = 0;
    assert(chunkBlockNum == 4);
    for(int i=0; i<chunkBlockNum; i++){
        EraseByAddress(BLOCK, startAddr);
        startAddr += BLOCK_SIZE;
    }
    for(int j=0 ; j<chunkSectorNum; j++){
        EraseByAddress(SECTOR, startAddr);
        startAddr += SECTOR_SIZE;
    }
}

int main(){
    unsigned long file_size = 46;
    Erase(file_size);
    return 0;
}

