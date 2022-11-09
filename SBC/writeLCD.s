.include "macros.s" @ Inclui no arquivo os macros e as pinagens necessárias.
.global writeChar

@ Escreve um char no display.
writeChar
        movs r10, r0 @Coluna
        WriteLCDFull r10
        bx lr