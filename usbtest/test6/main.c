#include <stdio.h>


typedef struct comms_packet_t {
    char operationCode[8];
    char data[256];
  } comms_packet_t;


comms_packet_t temporary_packet = { .operationCode = {0}, .data = {0}};


static char file_name[256];


void setFileName(const char* name) {
    strncpy(file_name, name, sizeof(file_name) - 1);
    file_name[sizeof(file_name) - 1] = '\0'; // Null -Terminate 추가
    printf("%s",file_name);
    
}

const char* get_file_name() {
    return file_name;
}

void process_uart_data(void) {
	switch(temporary_packet.operationCode[0]){
		case 'f':{ 
            printf("operation code is f\r\n");
            printf("filename: %s",temporary_packet.data);
            setFileName(temporary_packet.data);

        }break;
		default:{
		}break;
	}
}

int main(){

    temporary_packet.operationCode[0] = 'f';
    strcpy(temporary_packet.data , "abc.bin");
    process_uart_data();
    //printf("%s",temporary_packet.data);
    return 0;
}