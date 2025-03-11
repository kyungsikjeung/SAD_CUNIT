/*
 * author: datamining7830@gmail.com
 * Unit Test: GDB Test
 * Purpose: 
 * Description: 
 */
void foo(int *a){
    *a = *a +1;
    *a =10;
}


void bar(int *a){
    *a = *a +1;
    foo(a);
}


int main(){
    int a = 1;
    a = a+1;
    bar(&a);
    printf("Hello from test1\n");
    return 0;
}