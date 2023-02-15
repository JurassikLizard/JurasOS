# Declare a header as in the Multiboot Standard.
#.align 8
.section .multiboot
header_start:
	.long 0xE85250D6 # Magic
	.long 0 # Arch
	.long header_end - header_start # Header length

	.long -(0xE85250D6 + 0 + (header_end - header_start)) # Checksum

	# required end tag
    .short 0    # type
    .short 0    # flags
    .long 8    # size
header_end:

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
/*.type _start, @function*/
_start:
	movl	$stack_top, %esp

	# For multiboot info
	pushl	%ebx
	# Multiboot magic
	pushl	%eax

	# Call the global constructors.
	call	_init

	
	# Transfer control to the main kernel.
	call	kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start