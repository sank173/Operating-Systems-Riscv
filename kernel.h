#include "common.h"

#define PROCS_MAX 8       // Maximum number of processes

#define PROC_UNUSED   0   // Unused process control structure
#define PROC_RUNNABLE 1   // Runnable process

#define SATP_SV32 (1u << 31)
#define PAGE_V (1 << 0)    // Valid bit , entry is enabled
#define PAGE_R (1 << 1)    // Readable
#define PAGE_W (1 << 2)    // Writable 
#define PAGE_X (1 << 3)    // Exceutable
#define PAGE_U (1 << 4)    // User(accessible in user mode)
#define SSTATUS_SPIE (1 << 5) 

/**
 * This is the virtual base address of the application image and should match the base address specified in the user.ld
*/
#define USER_BASE 0x1000000 



struct process {
    int pid;             // Process ID
    int state;           // Process state: PROC_UNUSED or PROC_RUNNABLE
    vaddr_t sp;          // Stack pointer
    uint32_t *page_table;
    uint8_t stack[8192]; // Kernel stack
};

struct trap_frame{
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t sp;
} __attribute__((packed));

// by __attribute__((packed)) you are telling the compiler Do not add any secret padding. Squish these variables together as tightly as possible


#define READ_CSR(reg) \
 ({ \
    unsigned long __tmp;     \
    __asm__ __volatile__("csrr %0," #reg  : "=r"(__tmp)); \
    __tmp; \
 }) 

 #define WRITE_CSR(reg,value) \
 do { \
    uint32_t __tmp = (value); \
    __asm__ __volatile__("csrw " #reg ", %0" :: "r"(__tmp)); \
 }while(0)


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

