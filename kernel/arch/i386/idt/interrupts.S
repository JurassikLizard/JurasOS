.macro no_error_code_isr int_no
.global isr\int_no
isr\int_no: 
    cli
    push    $0                          # push 0 as error code
    pushl   $\int_no                    # push the interrupt number
    jmp     isr_common_stub    # jump to the common handler
.endm

.macro error_code_isr int_no
.global isr\int_no
isr\int_no: 
    cli
    pushl   $\int_no                    # push the interrupt number
    jmp     isr_common_stub    # jump to the common handler
.endm

isr_common_stub:                # the common parts of the generic interrupt handler
    pushal                    # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    movw %ds,%ax             # Lower 16-bits of eax = ds.
    pushl %eax               # save the data segment descriptor

    movw $0x10,%ax # load the kernel data segment descriptor
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs

    call fault_handler

    popl %eax      # reload the original data segment descriptor
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs

    popal                     # Pops edi,esi,ebp...
    addl $8,%esp   # Cleans up the pushed error code and pushed ISR number
    sti
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP


.macro asm_irq_handler irq_no irq_name
.global irq\irq_name
irq\irq_name:
    push    $0                    
    pushl   $\irq_no
    jmp     irq_common_stub    # jump to the common handler
.endm

irq_common_stub:
    pushal                    # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    movw %ds,%ax             # Lower 16-bits of eax = ds.
    pushl %eax               # save the data segment descriptor

    movw $0x10,%ax # load the kernel data segment descriptor
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs

    call irq_handler

    popl %eax               # save the data segment descriptor
    movw $0x10,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs

    # popl %ebx      # reload the original data segment descriptor
    # movw $0x10,%bx
    # movw %bx,%ds
    # movw %bx,%es
    # movw %bx,%fs
    # movw %bx,%gs

    popal                     # Pops edi,esi,ebp...
    addl $8,%esp   # Cleans up the pushed error code and pushed ISR number
    sti
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP


no_error_code_isr 0
no_error_code_isr 1
no_error_code_isr 2
no_error_code_isr 3
no_error_code_isr 4
no_error_code_isr 5
no_error_code_isr 6
no_error_code_isr 7
error_code_isr 8
no_error_code_isr 9
error_code_isr 10
error_code_isr 11
error_code_isr 12
error_code_isr 13
error_code_isr 14
no_error_code_isr 15
no_error_code_isr 16
no_error_code_isr 17
no_error_code_isr 18
no_error_code_isr 19
no_error_code_isr 20
no_error_code_isr 21
no_error_code_isr 22
no_error_code_isr 23
no_error_code_isr 24
no_error_code_isr 25
no_error_code_isr 26
no_error_code_isr 27
no_error_code_isr 28
no_error_code_isr 29
no_error_code_isr 30
no_error_code_isr 31

# Offset by 32
asm_irq_handler 32 0
asm_irq_handler 33 1
asm_irq_handler 34 2
asm_irq_handler 35 3
asm_irq_handler 36 4
asm_irq_handler 37 5
asm_irq_handler 38 6
asm_irq_handler 39 7
asm_irq_handler 40 8
asm_irq_handler 41 9
asm_irq_handler 42 10
asm_irq_handler 43 11
asm_irq_handler 44 12
asm_irq_handler 45 13
asm_irq_handler 46 14
asm_irq_handler 47 15