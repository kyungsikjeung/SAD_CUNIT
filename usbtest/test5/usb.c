#include "usb.h"
#include "uartdriver.h"



// USB 드라이버 Dummy FOO
void f_open(const char* path){
    printf("Opening file: %s\n", path);
}


void startDummyFoo(){
    // Do process  
    f_open(get_file_name());  
}