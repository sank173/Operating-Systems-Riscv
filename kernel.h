#pragma once 

struct  sbiret
{
    long error;
    long value;
};


// fmt is the short name for the format and is the first argument you pass to the PANIC and is thus only the variable name may be string
// \ is the line continuation character for continuting the defination of the macro to the next line 
// __LINE__ tells the source line in the source file
// __FILE__ tells the source file name
// ##__VA_ARGS__ --> takes the variable arguments to the printf and , ## , removes the preceding , if the number of arguments is empty
#define PANIC(fmt , ...)               \
   do{                                 \
   printf("PANIC: %s:%d: " fmt "\n",__FILE__,__LINE__, ##__VA_ARGS__); \
   while(1){}  \
} while (0) \

