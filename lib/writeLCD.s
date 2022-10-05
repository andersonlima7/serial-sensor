@ Controla os pinos D4, D5, D6, D7 e RS do display
@ R0 - Valor de 5 bits para acionar ou não determinado pino.
@ R4 - Copia o valor de R0


.include "turn.s"
.global WriteLCD


WriteLCD:
        push {r4, r9} 
        mov r4, r0
        mov r9, #0b00001      
        and r9, r4          @0001 & 0011 -> 0001
        GPIOTurn D4, r9

        @ D5
        mov r9, #0b00010   
        and r9, r4          @ 0010 & 0011 -> 0010
        lsr r9, #1              @ Desloca o bit 1x para direita  -> 0001
        GPIOTurn D5, r9

        @ D6
        mov r9, #0b00100      
        and r9, r4          @ 0100 & 0101 -> 0100
        lsr r9, #2              @ Desloca o bit 2x para direita  -> 0001
        GPIOTurn D6, r9

        @ D7
        mov r9, #0b01000      
        and r9, r4          @ 01000 & 01000 -> 01000
        lsr r9, #3              @ Desloca o bit 3x para direita  -> 00001
        GPIOTurn D7, r9

        @ RS
        mov r9, #0b10000       
        and r9, r4          @ 10000 & 10100 -> 10000
        lsr r9, #4              @ Desloca o bit 4x para direita  -> 00001
        GPIOTurn RS, r9
        enable
        pop {r4, r9}
        bx lr
        .ltorg

