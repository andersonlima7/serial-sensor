.equ setregoffset, 28 @ SET 0x 7E20 001C = 28
.equ clrregoffset, 40 @ CLEAR 0x 7E20 0028 = 40
.equ sys_nanosleep, 162

.global initLCD
.global writeChar
.global writeDigit
.global clearLCD 
.global writeNumber


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

@ Define o pino como saída
@ Autor: Sthepen Smith
.macro GPIODirectionOut pin
        ldr r2, =\pin @ offset of select register 
        ldr r2, [r2] @ load the value  GPFSEL0 - GPFSEL1 - GPFSEL2  
        ldr r1, [r8, r2] @ address of register -> 001 000 000 000 000 000 000 000 001 000 (Digamos que o pino1 e pino9 estejam como saidas)
        ldr r3, =\pin @ address of pin table 
        add r3, #4 @ load amount to shift from table Acessa a word para saber qual pino deve ser selecionado
        ldr r3, [r3] @ load value of shift amt FSEL0 - FSEL1 - FSEL2... FSEL25
        mov r0, #0b111 @ mask to clear 3 bits -> 111
        lsl r0, r3 @ shift into position ->        000 000 000 111 000 000 000 000 000 000 (posicao 18 - para o pino6 por exemplo)
        bic r1, r0 @ clear the three bits ->       001 000 000 000 000 000 000 000 001 000 Limpa os bits somente da posicao selecionada
        mov r0, #1 @ 1 bit to shift into pos 
        lsl r0, r3 @ shift by amount from table -> 000 000 000 001 000 000 000 000 000 000 (Coloca 1 bit na posicao certa)
        @ para definir o pino6 como saida, por exemplo.
        orr r1, r0 @ set the bit -> Define o bit no r1 001 000 000 001 000 000 000 000 001 000 (pino6 agora ativo)
        str r1, [r8, r2] @ save it to reg to do work -> Salva no registrador para executar o comando
        .ltorg
.endm

.macro SetOutputs
        GPIODirectionOut D4
        GPIODirectionOut D5
        GPIODirectionOut D6
        GPIODirectionOut D7
        GPIODirectionOut RS
        GPIODirectionOut E
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

@ Controla os pinos D4, D5, D6, D7 e RS do display
.macro Write value1 value2
       mov r4, #0b10000
       orr r4, \value1
       WriteLCD r4
       mov r4, #0b10000
       orr r4, \value2
       WriteLCD \value2
.endm



@Escreve um número no display a partir do valor informado deste mesmo número
.macro WriteNumber value
        @ value - 4 bits do número em binário.

        @ Seleciona as colunas de dígito do display
        WriteLCD #0b10011 

        mov r4, #0b10000 @ Deixa o bit correspondente ao pino do RS ligado, isto é, deixa o LCD em modo dados.
        orr r4, \value          @ 10000 OR 10110 (exemplo) = 10110 
        WriteLCD r4
        .ltorg
.endm


@ Limpa o display e retorna o cursor para a posição inicial
.macro clearLCD
        @ 0 0 0 0 0
        GPIOTurnOff D4
        GPIOTurnOff D5
        GPIOTurnOff D6
        GPIOTurnOff D7
        GPIOTurnOff RS
        enable

        @ 0 0 0 0 1
        GPIOTurnOn D4
        GPIOTurnOff D5
        GPIOTurnOff D6
        GPIOTurnOff D7
        GPIOTurnOff RS
        enable

        @WriteLCD #0b00000
        @WriteLCD #0b00001
.endm

@ Desloca o cursor ou o display para esquerda/direita.
@ SC - Display (1)  Cursor (0)
@ RL - Direita (1)  Esquerda (0)
.macro cursorDisplayShift SC RL
        GPIOTurnOn D4
        GPIOTurnOff D5
        GPIOTurnOff D6
        GPIOTurnOff D7
        GPIOTurnOff RS
        enable
        @WriteLCD #0b00001

        GPIOTurnOff D4 @ Não importa
        GPIOTurnOff D5 @ Não importa
        GPIOTurn D6, \RL @D6 - R/L Direita ou Esquerda
        GPIOTurn D7, \SC @D7 - Display ou Cursor
        GPIOTurnOff RS
        enable
.endm



.macro Initialization
        WriteLCD #0b00011 @Function set
        nanoSleep time5ms
        WriteLCD #0b00011 @Function set
        nanoSleep time100us
        WriteLCD #0b00011 @Function set

        @ Function Set
        WriteLCD #0b00010

        @ Function Set
        WriteLCD #0b00010
        WriteLCD #0b00000

        @ Display Off
        WriteLCD #0b00000
        WriteLCD #0b01000

        @ Clear
        clearLCD

        @ Entry Mode Set
        WriteLCD #0b00000
        WriteLCD #0b00110

        @Display On
        WriteLCD #0b00000
        WriteLCD #0b01110

        @ Entry Mode Set
        WriteLCD #0b00000
        WriteLCD #0b00110
        .ltorg
.endm

@ Divisão de inteiros
@ N - Numerador
@ D - Denominador
.macro division N D
    @ r10 - resultado
    @ r11 - resto
    @ r12 - denominador
    mov r10, #0 
    mov r11, \N
    mov r12, \D 
    bl loopDivision
.endm

@ Realiza a divisão como o dividendo atual
loopDivision:
        cmp r11, r12
        bxlo lr @ Condição de parada -> r1 = resto < r2 = denominador
        sub r11, r12
        add r10, #1
        b loopDivision
        

initLCD:
        SetOutputs
        Initialization
        bx lr

writeChar:
        movs r10, r0 @Coluna
        WriteLCDFull r10
        bx lr

writeDigit:
        movs r10, r0 @ Número
        WriteNumber r10
        bx lr

clearLCD:
        clearLCD
        bx lr


writeNumber:
        push {lr}
        mov r5, r0
        bl divisions
        WriteNumber r10
        pop {lr}
        bx lr
        
divisions:
        push {lr} @coloca na pilha o registrador LR (Link Register) - cria um indicador de retorno na pilha para onde o PC deve voltar após a execução
        SetOutputs
        mov r7, #10
        division r5, r7         @ realiza a divisao do valor dividendo atual por 10
        WriteNumber r11         @ escreve o 3 | escreve o 2
        cursorDisplayShift #0, #0 @Desloca o cursor para esquerda
        cursorDisplayShift #1, #1 @Desloca todo o conteudo do display para a direita (liberando a esquerda)
        cursorDisplayShift #0, #0 @Desloca o cursor para a esquerda
        
        mov r5, r10 @atualiza o dividendo atual para que seja o resultado da divisao realizada
        cmp r5, #10 @compara o dividendo com 10
        pop {lr} @remove o registrador LR da pilha
        bxlo lr         @ casoo valor  r5 < 10 entao retorna para o ponto indicado pelo link register
        b divisions @ senao continua fazendo a divisao


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


mask16: 

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