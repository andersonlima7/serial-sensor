.equ setregoffset, 28 @ SET 0x 7E20 001C = 28
.equ clrregoffset, 40 @ CLEAR 0x 7E20 0028 = 40
.equ sys_nanosleep, 162

.global writeChar

@ Delay
.macro nanoSleep totime
        LDR R0,=\totime
        LDR R1,=\totime
        MOV R7, #sys_nanosleep
        SWI 0
.endm

.macro enable
        GPIOTurnOff E
        nanoSleep time1ms
        GPIOTurnOn E
        nanoSleep time1ms
        GPIOTurnOff E
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

@ Define o pino como nível lógico alto.
@ Autor: Sthepen Smith
.macro GPIOTurnOn pin
         mov r2, r8 @ address of gpio regs
         add r2, #setregoffset @ off to set reg
         mov r0, #1 @ 1 bit to shift into pos
         ldr r3, =\pin @ base of pin info table
         add r3, #8 @ add offset for shift amt
         ldr r3, [r3] @ load shift from table
         lsl r0, r3 @ do the shift
         str r0, [r2] @ write to the register
.endm

@ Define o pino como nível lógico baixo.
@ Autor: Sthepen Smith
.macro GPIOTurnOff pin
         mov r2, r8 @ address of gpio regs
         add r2, #clrregoffset @ off set of clr reg
         mov r0, #1 @ 1 bit to shift into pos
         ldr r3, =\pin @ base of pin info table
         add r3, #8 @ add offset for shift amt
         ldr r3, [r3] @ load shift from table
         lsl r0, r3 @ do the shift
         str r0, [r2] @ write to the register
.endm


writeChar:
        movs r10, r0 @Coluna
        @ Coluna
        @ D4
        mov r9, #0b00010000     @16 
        and r9, r10         @0001 & 0011 -> 0001
        lsr r9, #4
        GPIOTurn D4, r9

        @ D5
        mov r9, #0b00100000     @32
        and r9, r10         @ 0010 & 0011 -> 0010
        lsr r9, #5              @ Desloca o bit 1x para direita  -> 0001
        GPIOTurn D5, r9

        @ D6
        mov r9, #0b01000000     @64
        and r9, r10         @ 0100 & 0101 -> 0100
        lsr r9, #6              @ Desloca o bit 2x para direita  -> 0001
        GPIOTurn D6, r9

        @ D7
        mov r9, #0b10000000     @128
        and r9, r10         @ 01000 & 01000 -> 01000
        lsr r9, #7              @ Desloca o bit 3x para direita  -> 00001
        GPIOTurn D7, r9

        @ RS
        GPIOTurnOn RS
        enable

        @ Linha
        @ D4
        mov r9, #0b00000001      
        and r9, r10         @0001 & 0011 -> 0001
        GPIOTurn D4, r9

        @ D5
        mov r9, #0b00000010   
        and r9, r10         @ 0010 & 0011 -> 0010
        lsr r9, #1              @ Desloca o bit 1x para direita  -> 0001
        GPIOTurn D5, r9

        @ D6
        mov r9, #0b00000100      
        and r9, r10         @ 0100 & 0101 -> 0100
        lsr r9, #2              @ Desloca o bit 2x para direita  -> 0001
        GPIOTurn D6, r9

        @ D7
        mov r9, #0b00001000      
        and r9, r10         @ 01000 & 01000 -> 01000
        lsr r9, #3              @ Desloca o bit 3x para direita  -> 00001
        GPIOTurn D7, r9

        @ RS
        GPIOTurnOn RS
        enable
        bx lr


.data


time5ms: .word 0
         .word 5000000
time1ms: .word 0
         .word 1000000
time100us:.word 0
          .word 150000
time450ns:
        .word 0
        .word 450
time1s: .word 1
        .word 000000000




@ E - Enable 
@ Esse pino é usado para habilitar o LCD quando um pulso de nível lógico alto para baixo é dado por ele.
E: .word 0 @ GPFSEL0
        .word 3 @FSEL1
        .word 1 @ pino 1 para SET e CLEAR


@ Data Pins, Stores the Data to be displayed on LCD or the command instructions

@ D4
D4: .word 4 @ GPFSEL1
        .word 6 @FSEL12
        .word 12 @ pino 12 para SET e CLEAR

@ D5
D5: .word 4 @ GPFSEL1
        .word 18 @FSEL16
        .word 16 @ pino 16 para SET e CLEAR

@ D6
D6: .word 8 @ GPFSEL2
        .word 0 @FSEL20
        .word 20 @ pino 20 para SET e CLEAR

@ D7
D7: .word 8 @ GPFSEL2
        .word 3 @FSEL21
        .word 21 @ pino 21 para SET e CLEAR

@RS - Register Select Pin, RS=0 Command mode, RS=1 Data mode
@ Definimos como 1 se estivermos mandando dados para ser escrito no display.
@ Definimos como 0 se estivermos mandando alguma instução de comando como limpar o display.
RS: .word 8 @ GPFSEL2
        .word 15 @FSEL25
        .word 25 @ pino 25 para SET e CLEAR

.text