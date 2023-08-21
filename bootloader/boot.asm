bits 16

org 0x7C00

_start:
    xor ax, ax
    jmp loop

exit:
    hlt

loop:
    add ax, 0x1
    jmp exit
