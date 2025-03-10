
#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include <stdio.h>
#include <string.h>



// 헤더 정의
static char file_name[256];
typedef unsigned short WCHAR; 
typedef WCHAR TCHAR;


void setFileName(const char* name);
const char* get_file_name();

#endif /* UART_DRIVER_H_ */