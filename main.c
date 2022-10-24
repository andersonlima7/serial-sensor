
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
    int length = strlen(string);
    for (int i = 0; i< length; i++){
        writeChar(string[i]);
    }
}

int main()
{
    mapMem();
    initLCD();
    // writeNumber(123);
    // writeChar('S');
    // writeChar('i');
    // writeChar('m');
    // writeChar('a');
    // writeChar('o');
    writeLCD("Sensor1: 10");

    return(0);
}
