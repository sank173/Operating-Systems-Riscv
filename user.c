#include "user.h"

extern char __stack_top[];
/**
 * the no return attribute tells that this function will never return to the function that called it 
 * Thus such functions should always have a  infinite loop or else it give the error as it returns
*/
__attribute__((noreturn)) void exit(void){
    for(;;);
}

void putchar(char ch){

}

__attribute__((section(".text.start")))
__attribute__((naked))
void start(void){
    __asm__ __volatile__(
        "mv sp,%[stack_top] \n"
        "call main          \n"
        "call exit          \n"
        :: [stack_top] "r" (__stack_top)
    );
}

