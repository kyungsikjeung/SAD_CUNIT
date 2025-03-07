/*
 * author: datamining7830@gmail.com
 * Unit Test:   파일크기에 따른 256바이트 Chunk 확인
 * Purpose:     파일 크기 Streaming 형식으로 전송 해야함. 
 * Description:  256바이트 단위로 Chunk로 나누어서 전송, Print  
 */

 #include <stdio.h>
 #include <stdint.h>
 /// aaa.bin 201 Kbyte
 unsigned long fileSize = 513;
 int chunkSize = 256;
 int main(){
    uint8_t testBuffer[256];
    for(int i=0; i<256;i++){
        testBuffer[i] = i;
    }

    int chunkNum = fileSize / 256 + 1;
    unsigned long long cnt =0;
    printf("chunkNum: %d\n", chunkNum);
    printf("\r\n0\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\r\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------\r\n"); 


    for(int i=0; i<chunkNum;i++){
       
        for(int j=0; j<256;j++){
            printf(" %d\t", testBuffer[j]);
            cnt++;
            if(j % 16 == 0 ){
                printf("\n");
            }
        }
        


        printf("\r\n-----------------------------------------------------------------------------------------------------------------------------\r\n"); 

    }
    printf("chunk Total Num: %d\n", chunkNum);

 }