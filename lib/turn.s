@ Liga/Desliga um pino de saída.
@ R0 - Pino
@ R1 - Valor(0/1)

.equ setregoffset, 28 @ SET 0x 7E20 001C = 28
.equ clrregoffset, 40 @ CLEAR 0x 7E20 0028 = 40

.include "pins.s"

.global turn

@ Define o pino como saída
@ Autor: Sthepen Smith
.macro GPIODirectionOut pin
        ldr r2, =\pin 
        ldr r2, [r2] 
        ldr r1, [r8, r2] 
        ldr r3, =\pin 
        add r3, #4 
        ldr r3, [r3] 
        mov r0, #0b111 
        lsl r0, r3 
        bic r1, r0 
        mov r0, #1  
        lsl r0, r3 
        orr r1, r0 
        str r1, [r8, r2] 
        .ltorg
.endm


@ Define o pino como nível lógico alto ou baixo.
.macro GPIOTurn pin value
        mov r2, r8
        mov r0, \value
        cmp r0, #0
        addeq r2, #clrregoffset
        addne r2, #setregoffset
        mov r0, #1 @ 1 bit to shift into pos
        ldr r3, =\pin @ base of pin info table
        add r3, #8 @ add offset for shift amt
        ldr r3, [r3] @ load shift from table
        lsl r0, r3 @ do the shift
        str r0, [r2] @ write to the register
.endm


turn:
    push {r4, r5}
    mov r4, r0
    mov r5, r1
    GPIODirectionOut r4
    GPIOTurn r4, r5
    pop {r4, r5}
    bx lr

