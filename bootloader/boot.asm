org 0xFE000

bits 16
cpu 8086

_start:
    xor ax, ax      ; Clear AX register (AX = 0)
    mov bx, 5       ; Set BX to 5
    add ax, bx      ; Add BX to AX (AX = 5)

    cmp ax, bx      ; Compare AX and BX
    je equal        ; Jump if equal (ZF flag set)
    hlt             ; Halt

end:
    hlt

equal:
    add ax, 1       ; Add 1 to AX
    jmp end