@ Função para fazer o mapeamento dos pinos na memória.
@ R8 - Retorno: Endereço de memória base da GPIO. 

.equ pagelen, 4096
.equ PROT_READ, 1
.equ PROT_WRITE, 2
.equ MAP_SHARED, 1
.equ sys_mmap2, 192

.global mapMem


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

mapMem: 
    ldr r0, =devmem
    mov r1, #0x1b0
    orr r1, #0x006
    mov r2, r1
    @ Permissão de Leitura/Escrita
    mov r7, #5
    svc 0
    movs r4, r0         @ fd for memmap
    ldr r5, =gpioaddr   @ Endereço pretendido.
    ldr r5, [r5]        @ Carrega o endereço.
    mov r1, #pagelen    @ Tamanho da memória que queremos.
    @ opções de prroteção de memória
    mov r2, #(PROT_READ + PROT_WRITE) 
    mov r3, #MAP_SHARED @ opções de compartilhamento
    mov r0, #0          @ Permite o linux escolher o endereço virtual de memória.
    mov r7, #sys_mmap2  @ Serviço mmap2
    svc 0 @ chama o serviço
    movs r8, r0 @ keep the returned virt addr

    GPIODirectionOut D4
    GPIODirectionOut D5
    GPIODirectionOut D6
    GPIODirectionOut D7
    GPIODirectionOut RS
    GPIODirectionOut E
    bx lr @ Retorno da função



.data
devmem: .asciz "/dev/mem"
@ Endereço de memória dos registradores GPIO.
gpioaddr: .word 0x20200


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
