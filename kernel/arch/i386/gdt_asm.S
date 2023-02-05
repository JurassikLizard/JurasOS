/***************** Data *****************/
.data

/***************** Text *****************/
.text

.set gdtr, .
    .word   0 # For limit storage
    .long   0 # For base storage

.global load_gdt
load_gdt:
    movw    4(%esp),%ax
    movw    %ax,gdtr
    movl    8(%esp),%eax
    movl    %eax,gdtr + 2
    lgdt    gdtr
    ret

# Reload CS register containing code selector:
reloadSegments: 
jmp   $0x08,$reload_CS # 0x08 is our code segment

reload_CS:
movw    $0x10,%ax # 0x10 is a stand-in for your data segment
movw    %ax,%ds
movw    %ax,%es
movw    %ax,%fs
movw    %ax,%gs
movw    %ax,%ss
ret

