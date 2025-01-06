#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef enum {
    Mon,
    Tuesday,
    Wednesday,
    Thurday,
    Firday
} Day;


int main(){
    long test = strtol("0x10000", NULL, 16);
    // printf("Hello World %\n", test);
    printf("Hello World %ld\n", test);
    printf("Hello World\n");
    printf("max is %d\n", MAX);
    printf("max is %d\n", Mon);
    Day day = Mon;
    if(day == 0){
        printf("Mon is 0\n");
    }


    return 0;
}