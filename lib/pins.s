.data

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