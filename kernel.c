#include "kernel.h"
#include "common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];


__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp) {
    __asm__ __volatile__(
        // Save callee-saved registers onto the current process's stack.
        "addi sp, sp, -13 * 4\n" // Allocate stack space for 13 4-byte registers
        "sw ra,  0  * 4(sp)\n"   // Save callee-saved registers only
        "sw s0,  1  * 4(sp)\n"
        "sw s1,  2  * 4(sp)\n"
        "sw s2,  3  * 4(sp)\n"
        "sw s3,  4  * 4(sp)\n"
        "sw s4,  5  * 4(sp)\n"
        "sw s5,  6  * 4(sp)\n"
        "sw s6,  7  * 4(sp)\n"
        "sw s7,  8  * 4(sp)\n"
        "sw s8,  9  * 4(sp)\n"
        "sw s9,  10 * 4(sp)\n"
        "sw s10, 11 * 4(sp)\n"
        "sw s11, 12 * 4(sp)\n"

        // Switch the stack pointer.
        "sw sp, (a0)\n"         // *prev_sp = sp;
        "lw sp, (a1)\n"         // Switch stack pointer (sp) here

        // Restore callee-saved registers from the next process's stack.
        "lw ra,  0  * 4(sp)\n"  // Restore callee-saved registers only
        "lw s0,  1  * 4(sp)\n"
        "lw s1,  2  * 4(sp)\n"
        "lw s2,  3  * 4(sp)\n"
        "lw s3,  4  * 4(sp)\n"
        "lw s4,  5  * 4(sp)\n"
        "lw s5,  6  * 4(sp)\n"
        "lw s6,  7  * 4(sp)\n"
        "lw s7,  8  * 4(sp)\n"
        "lw s8,  9  * 4(sp)\n"
        "lw s9,  10 * 4(sp)\n"
        "lw s10, 11 * 4(sp)\n"
        "lw s11, 12 * 4(sp)\n"
        "addi sp, sp, 13 * 4\n"  // We've popped 13 4-byte registers from the stack
        "ret\n"
    );
}

paddr_t alloc_pages(uint32_t n){
    /* make the next_addr variable non changeable between the function calls , thus making it kind of global variable*/
    
    static paddr_t next_addr = (paddr_t) __free_ram;
    paddr_t paddr = next_addr;
    next_addr += n*PAGE_SIZE;
    if(next_addr > __free_ram_end){
         PANIC("Out of Memory\n");
    }
    memset((void *) paddr , 0 , n*PAGE_SIZE);
    return paddr;

}

/**
 * sscratch register is used as a temporary storage to save the stack pointer at the time of exception occurrence, which is later restored.
 * Floating-point registers are not used within the kernel, and thus there's no need to save them here. Generally, they are saved and restored during thread switching.
*/
__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void){
    __asm__ __volatile__(
        "csrrw sp , sscratch , sp\n"
        "addi sp, sp, -4*31\n"
        "sw ra, 4* 0(sp)\n"
        "sw gp, 4* 1(sp)\n"
        "sw tp, 4* 2(sp)\n"
        "sw t0, 4* 3(sp)\n"
        "sw t1, 4* 4(sp)\n"
        "sw t2, 4* 5(sp)\n"
        "sw t3, 4* 6(sp)\n"
        "sw t4, 4* 7(sp)\n"
        "sw t5, 4* 8(sp)\n"
        "sw t6, 4* 9(sp)\n"
        "sw a0, 4* 10(sp)\n"
        "sw a1, 4* 11(sp)\n"
        "sw a2, 4* 12(sp)\n"
        "sw a3, 4* 13(sp)\n"
        "sw a4, 4* 14(sp)\n"
        "sw a5, 4* 15(sp)\n"
        "sw a6, 4* 16(sp)\n"
        "sw a7, 4* 17(sp)\n"
        "sw s0, 4* 18(sp)\n"
        "sw s1, 4* 19(sp)\n"
        "sw s2, 4* 20(sp)\n"
        "sw s3, 4* 21(sp)\n"
        "sw s4, 4* 22(sp)\n"
        "sw s5, 4* 23(sp)\n"
        "sw s6, 4* 24(sp)\n"  
        "sw s7, 4* 25(sp)\n"
        "sw s8, 4* 26(sp)\n"
        "sw s9, 4* 27(sp)\n"
        "sw s10,4* 28(sp)\n"
        "sw s11,4* 29(sp)\n"

        "csrr a0,sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        "addi a0, sp , 4*31\n"
        "csrw sscratch ,a0\n"
/* The first argument to a function must be in register a0 The First argument should be in a0 before calling the handle_trap*/
        "mv a0 , sp \n"
        "call handle_trap\n"

        "lw ra, 4* 0(sp)\n"
        "lw gp, 4* 1(sp)\n"
        "lw tp, 4* 2(sp)\n"
        "lw t0, 4* 3(sp)\n"
        "lw t1, 4* 4(sp)\n"
        "lw t2, 4* 5(sp)\n"
        "lw t3, 4* 6(sp)\n"
        "lw t4, 4* 7(sp)\n"
        "lw t5, 4* 8(sp)\n"
        "lw t6, 4* 9(sp)\n"
        "lw a0, 4* 10(sp)\n"
        "lw a1, 4* 11(sp)\n"
        "lw a2, 4* 12(sp)\n"
        "lw a3, 4* 13(sp)\n"
        "lw a4, 4* 14(sp)\n"
        "lw a5, 4* 15(sp)\n"
        "lw a6, 4* 16(sp)\n"
        "lw a7, 4* 17(sp)\n"
        "lw s0, 4* 18(sp)\n"
        "lw s1, 4* 19(sp)\n"
        "lw s2, 4* 20(sp)\n"
        "lw s3, 4* 21(sp)\n"
        "lw s4, 4* 22(sp)\n"
        "lw s5, 4* 23(sp)\n"
        "lw s6, 4* 24(sp)\n"  
        "lw s7, 4* 25(sp)\n"
        "lw s8, 4* 26(sp)\n"
        "lw s9, 4* 27(sp)\n"
        "lw s10,4* 28(sp)\n"
        "lw s11,4* 29(sp)\n"
        "sret\n"

    );
}
void map_page(uint32_t *table1 , uint32_t vaddr, paddr_t paddr , uint32_t flags){

    if(!is_aligned(vaddr,PAGE_SIZE)){
       PANIC("unaligned vaddr %x", vaddr);
    }
    if(!is_aligned(paddr,PAGE_SIZE)){
       PANIC("unaligned paddr %x", paddr);
    }

    uint32_t vpn1 =  (vaddr >> 22) & 0x3ff;
    if((table1[vpn1]  & PAGE_V) == 0){
      uint32_t pt_paddr = alloc_pages(1);
      table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10)  | PAGE_V;
    }

    uint32_t vpn0 = (vaddr >> 12) & 0x3ff;
    uint32_t *table0 = (uint32_t *) ((table1[vpn1] >> 10) * PAGE_SIZE);
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
} 


/*
* In Riscv Exception is handled in this way 
* The CPU checks for the medeleg register which decides which mode should handle the exception
* In this case OpenSBI is capabale of handling the S/U mode exception in s mode handler 
* CPU saves the registers in various CSRs
* The value of stvec is set to the PC(program counter) to jump to the exception handler 
* The exception handler saves the current register (gprs) and handles the exception
* exception handler restores the saved excecution state and calls the sret instruction to resume the exection from the point where execution occured

* Register Information
* scaue --> type of exception , tells the type of exception occured
* stval --> tells the exact adddress where the exception occured
* sepc --> the Program counter at which exception occured 
* sstatus --> tells the Operating mode (S/U mode) where the exception occurred
*/
void handle_trap(struct trap_frame *f){
    uint32_t scause = READ_CSR(scause);
    uint32_t stval = READ_CSR(stval);
    uint32_t user_pc = READ_CSR(sepc);
    PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n",scause,stval,user_pc);
}

struct sbiret sbi_ecall(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid)
{
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall" : "=r"(a0), "=r"(a1) : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");

    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch)
{
    sbi_ecall(ch, 0, 0, 0, 0, 0, 0, 1);
}
struct process procs[PROCS_MAX]; // All process control structures.

struct process *create_process(uint32_t pc) {
    // Find an unused process control structure.
    struct process *proc = NULL;
    int i;
    for (i = 0; i < PROCS_MAX; i++) {
        if (procs[i].state == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (!proc)
        PANIC("no free process slots");

    // Stack callee-saved registers. These register values will be restored in
    // the first context switch in switch_context.
    uint32_t *sp = (uint32_t *) &proc->stack[sizeof(proc->stack)];
    *--sp = 0;                      // s11
    *--sp = 0;                      // s10
    *--sp = 0;                      // s9
    *--sp = 0;                      // s8
    *--sp = 0;                      // s7
    *--sp = 0;                      // s6
    *--sp = 0;                      // s5
    *--sp = 0;                      // s4
    *--sp = 0;                      // s3
    *--sp = 0;                      // s2
    *--sp = 0;                      // s1
    *--sp = 0;                      // s0
    *--sp = (uint32_t) pc;          // ra

    uint32_t *page_table = (uint32_t *) alloc_pages(1);
    for(paddr_t paddr = (paddr_t) __kernel_base ; paddr < (paddr_t) __free_ram_end ; paddr+=PAGE_SIZE)
    map_page(page_table, paddr , paddr , PAGE_R | PAGE_W | PAGE_X);
    

    // Initialize fields.
    proc->pid = i + 1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (uint32_t) sp;
    proc->page_table = page_table;
    return proc;
}

void delay(void) {
    for (int i = 0; i < 30000000; i++)
        __asm__ __volatile__("nop"); // do nothing
}



struct process *proc_a;
struct process *proc_b;
struct process *current_proc;
struct process *idle_proc;

void yeild(void){
    struct process * next = idle_proc;
    for(int i =0;i<PROCS_MAX;i++){
         struct process *proc = &procs[(current_proc->pid + i) %PROCS_MAX];
         if(proc->state == PROC_RUNNABLE && proc->pid > 0){
            next = proc;
            break;
         }
         
    }
    if(next == current_proc){
         return;
    }

    __asm__ __volatile__(
        "sfence.vma\n"
        "csrw  satp , %[satp]\n"
        "sfence.vma\n"
        "csrw sscratch , %[sscratch]\n"
        :
        :[satp] "r" (SATP_SV32 | (uint32_t) next->page_table / PAGE_SIZE) , [sscratch] "r" ((uint32_t) &next->stack[sizeof(next->stack)])
    );

    struct process *prev = current_proc;
    current_proc = next;
    switch_context(&prev->sp , &next->sp);
}


void proc_a_entry(void) {
    printf("starting process A\n");
    while (1) {
        putchar('A');
        yeild();
    }
}

void proc_b_entry(void) {
    printf("starting process B\n");
    while (1) {
        putchar('B');
        yeild();
    }
}
/**
 * llvm-addr2line -e kernel.elf 8020015e --> comannd line which tells at which line in the code the exception occured , use the sepc value
*/
void kernel_main(void)
{
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    WRITE_CSR(stvec, (uint32_t) kernel_entry);

    idle_proc = create_process((uint32_t) NULL);
    idle_proc->pid = 0;
    current_proc = idle_proc;

    proc_a = create_process((uint32_t) proc_a_entry);
    proc_b = create_process((uint32_t) proc_b_entry);
    /** 
     * Usual flow is that we yield , we make the next process as process A , and previous process is idle process, we enter the switch context , make ra(return address) as the proc_a_entry
     * now we enter the proc_a_entry and yeild again , we make the next process as B and previous process as A , switch context , move to process B , again yeild and making the process A as next and process B as previous process
     * printing ABABAB...
    */
    yeild();
    PANIC("switched to idle process");
   
}

/* place the function compiled to .text.boot section in the linker*/
__attribute__((section(".text.boot")))
/* do not generate the Prologue(startup) and epilogue(clean up) for this section as we do not have the stack pointer yet*/
__attribute__((naked)) void
boot(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer
        "j kernel_main\n"       // Jump to the kernel main function
        :
        : [stack_top] "r"(__stack_top) // Pass the stack top address as %[stack_top]
    );
}