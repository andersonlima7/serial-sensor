
#include <stdio.h>
#include <string.h>

extern void mapMem();
extern void initLCD();
extern void clearLCD();
extern void writeChar(char c);  //Escreve no display.


/**
 * Recebe uma string e escreve  no display.
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
    writeLCD("Sensor1: 10");

    return(0);
},
