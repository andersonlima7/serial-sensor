@ Macros necessárias para utilizar o display LCD.

.equ setregoffset, 28 @ SET 0x 7E20 001C = 28
.equ clrregoffset, 40 @ CLEAR 0x 7E20 0028 = 40
.equ sys_nanosleep, 162


@ Delay
.macro nanoSleep time
        ldr r0,=\time
        ldr r1,=\time
        mov r7, #sys_nanosleep
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
        mov r0, #40             @Move #40 para R0 (40 é o offset do clear register)
        mov r2, #12             @Move #12 para R2 (12 é a diferença entre os offsets do clear e do set registeRS)
        mov r1, \value            @Move para R1 o valor do nível lógico desejado
        mul r5, r1, r2          @Multiplica 12 pelo nível lógico recebido
        sub r0, r0, r5          @Subtrai 40 pelo resultado obtido na operação anterior
        mov r2, r8              @Move o endereço base dos GPIO obtido no mapeamento para o R2
        add r2, r2, r0          @Soma a esse endereço o offset calculado nas operações anteriores, podendo ser 28 (set register) ou 40 (clear register)
        mov r0, #1              @Move #1 para R0
        ldr r3, =\pin           @Carrega o endereço de memória contendo o offset do GPFSel em R3
        add r3, #8              @Adiciona 8 a esse endereço, para obter o endereço que contém a posição do bit responsável por definir o nível lógico daquele pino específico
        ldr r3, [r3]            @Carrega o valor contido nesse endereço em R3
        lsl r0, r3              @Desloca o bit colocado em R0 para a posição obtida na operação anterior
        str r0, [r2]            @Armazena no registrador de clear ou set o nível lógico do pino atualizado
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



@ Controla os pinos D4, D5, D6, D7 e RS do display
.macro WriteLCD value
        mov r9, #0b00001      
        and r9, \value          @0001 & 0011 -> 0001
        GPIOTurn D4, r9

        @ D5
        mov r9, #0b00010   
        and r9, \value          @ 0010 & 0011 -> 0010
        lsr r9, #1              @ Desloca o bit 1x para direita  -> 0001
        GPIOTurn D5, r9

        @ D6
        mov r9, #0b00100      
        and r9, \value          @ 0100 & 0101 -> 0100
        lsr r9, #2              @ Desloca o bit 2x para direita  -> 0001
        GPIOTurn D6, r9

        @ D7
        mov r9, #0b01000      
        and r9, \value          @ 01000 & 01000 -> 01000
        lsr r9, #3              @ Desloca o bit 3x para direita  -> 00001
        GPIOTurn D7, r9

        @ RS
        mov r9, #0b10000       
        and r9, \value          @ 10000 & 10100 -> 10000
        lsr r9, #4              @ Desloca o bit 4x para direita  -> 00001
        GPIOTurn RS, r9
        enable
        .ltorg
.endm

.macro WriteLCDFull value

        @ Coluna
        @ D4
        mov r9, #0b00010000     @16 
        and r9, \value          @0001 & 0011 -> 0001
        lsr r9, #4
        GPIOTurn D4, r9

        @ D5
        mov r9, #0b00100000     @32
        and r9, \value          @ 0010 & 0011 -> 0010
        lsr r9, #5              @ Desloca o bit 1x para direita  -> 0001
        GPIOTurn D5, r9

        @ D6
        mov r9, #0b01000000     @64
        and r9, \value          @ 0100 & 0101 -> 0100
        lsr r9, #6              @ Desloca o bit 2x para direita  -> 0001
        GPIOTurn D6, r9

        @ D7
        mov r9, #0b10000000     @128
        and r9, \value          @ 01000 & 01000 -> 01000
        lsr r9, #7              @ Desloca o bit 3x para direita  -> 00001
        GPIOTurn D7, r9

        @ RS
        GPIOTurnOn RS
        enable
        .ltorg


        @ Linha
        @ D4
        mov r9, #0b00000001      
        and r9, \value          @0001 & 0011 -> 0001
        GPIOTurn D4, r9

        @ D5
        mov r9, #0b00000010   
        and r9, \value          @ 0010 & 0011 -> 0010
        lsr r9, #1              @ Desloca o bit 1x para direita  -> 0001
        GPIOTurn D5, r9

        @ D6
        mov r9, #0b00000100      
        and r9, \value          @ 0100 & 0101 -> 0100
        lsr r9, #2              @ Desloca o bit 2x para direita  -> 0001
        GPIOTurn D6, r9

        @ D7
        mov r9, #0b00001000      
        and r9, \value          @ 01000 & 01000 -> 01000
        lsr r9, #3              @ Desloca o bit 3x para direita  -> 00001
        GPIOTurn D7, r9

        @ RS
        GPIOTurnOn RS
        enable
        .ltorg
.endm

@ Limpa o display e retorna o cursor para a posição inicial
.macro clearLCD
        WriteLCD #0b00000
        WriteLCD #0b00001
.endm

.data

tempoInicial:
        .word 999
        @4294967295

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