
#include <stdio.h>
#include <string.h>

extern void mapMem();
extern void initLCD();
extern void clearLCD();
extern void writeDigit(int number);         // Escreve de 0 a 9.
extern void writeNumber(int number);        // Escreve qualquer número.
extern void writeChar(char c);              // Escreve qualquer char;


/**
 * Recebe uma string e manda para o display.
*/
void writeLCD(char string[])  {
    int length = srtlen(string);
    for (int i = 0; i< length; i++){
        writeChar(string[i]);
    }
}

int main()
{
    mapMem();
    initLCD();
    // writeNumber(123);
    writeChar('T');
    // writeChar('i');
    // writeChar('m');
    // writeChar('a');
    // writeChar('o');
    writeLCD("Timao");

    return(0);
}
