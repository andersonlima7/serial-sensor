.include "macros.s" @ Inclui no arquivo os macros e as pinagens necessárias.
.global initLCD

@ Inicializa o Display
initLCD:
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
        bx lr