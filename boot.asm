%define ASM_FILE 
%include "multiboot.h"

;C symbol format.
%ifdef HAVE_ASM_UNSCORE
%define EXIT_C(sym)     __sym 
%else
%define EXIT_C(sym)     sym 
%endif 

;*size of stack(16KB)*/ 
%define STACK_SIZE      0x4000

;multiboot header flags 
%ifdef __ELF__ 
%define AOUT_KLUDGE     0 
%else 
%define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE 
%endif
%define MULTIBOOT_HEADER_FLAGS  MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE | _AOUT_KLUDGE  

section .text
    global start, _start 

start:

_start:

    jmp     multiboot_entry

    ;align to 32 bit s boundary 
    .align  4 

multiboot_header:

    ;magic 
    .dd     MULTIBOOT_HEADER_MAGIC 

    ;flags
    .dd     MULTIBOOT_HEADER_FLAGS

    ;checksum
    .dd     -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS);

%ifndef __ELF__
    
    ;header addr 
    .dd     multiboot_header

    ;load addr 
    .dd     _start

    ;load_end_addr 
    .dd     _edata 

    ;bss_end_addr 
    .dd     _end 
    
    ;entry addr
    ;where executiion begins 
    .dd     multiboot_entry 

%else ;  !__ELF__ 
    .dd     0 
    .dd     0 
    .dd     0 
    .dd     0 
    .dd     0 

%endif  
    .dd     0 
    .dd     1024 
    .dd     768 
    .dd     32 

multiboot_entry:

    ;initialize the stack pointer 
    mov     esp, stack + STACK_SIZE; 

    ;reset flags 
    push    0 
    popf 

    ;push ptr to multiboot info struct 
    push    ebx 

    ;push the magic value 
    push    eax 
    
    ;now enter c main function 
    call    EXT_C(cmain)

    ;halt 
    push    halt_message
    call    EXT_C(printf)

loop:
    hlt 
    jmp     loop 

halt_message:
    .asicz  "halted."

    ;stack area 
    .comm   stack, STACK_SIZE 
        






    

