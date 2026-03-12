typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

void kernel_main(void) {
    /* intializing the bss section with default 0s*/
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    for (;;);
}

/* place the function compiled to .text.boot section in the linker*/
__attribute__((section(".text.boot")))
/* do not generate the Prologue(startup) and epilogue(clean up) for this section as we do not have the stack pointer yet*/
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer
        "j kernel_main\n"       // Jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // Pass the stack top address as %[stack_top]
    );
}