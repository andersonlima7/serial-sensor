#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  //Usado para UART
#include <fcntl.h>   //Usado para UART
#include <termios.h> //Usado para UART

extern void mapMem();
extern void initLCD();
extern void clearLCD();
extern void writeChar(char c); // Escreve no display.

/**
 * Recebe uma string e escreve  no display.
 */
void writeLCD(char string[])
{
    int length = strlen(string);
    for (int i = 0; i < length; i++)
    {
        writeChar(string[i]);
    }
}

void uart_tx(unsigned char com, unsigned char addr, int uart_filestream); // Funcao para envio de dados
unsigned char uart_rx();                                                  // Funcao para recebimento de dados

#define SITUACAO_ATUAL 0x03    // codigo da situacao atual
#define ENTRADA_ANALOGICA 0x04 // codigo do valor da entrada analogica
#define SENSOR_DIGITAL 0x05    // codigo do valor da entrada digital
#define CONTROLAR_LED 0x06     // codigo para acender/desligar o led
int uart_filestream = -1;
int sensor = 0; // Armazena a opcao do sensor selecionado pelo usuário.

int main()
{
    mapMem();
    initLCD();
    writeLCD("Problema #2");
    clearLCD();
    int comando = 0; // Armazena a opcao de comando selecionado pelo usuário.

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Configuracao da UART

    // Abertura do arquivo da UART
    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Abre em modo escrita/leitura bloqueado
    if (uart_filestream == -1)
    {
        printf("\nErro na abertura da UART.\n");
    }
    // Struct para configuracao dos parametros de comunicacao
    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Seta Baud-Rate para 9600, tamanho de 8 bits e sem paridade
    options.c_iflag = IGNPAR;                       // Ignora caracteres com erros de paridade
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH); // Libera entrada pendente. Esvazia a saida nao transmitida.
    tcsetattr(uart_filestream, TCSANOW, &options);

    while (comando != -1)
    {

        // Solicitacao de dados para criar a requisicao
        // Solicita ao usuario o tipo de informacao que deseja receber do sensor
        printf("\nSelecione a requisição que deseja realizar");
        printf("\n1 - Solicitar a situação atual do NodeMCU\n2 - Solicitar o valor da entrada analógica\n3 - Solicitar o valor de uma das entradas digitais.\n4 - Controlar led da NodeMCU\n");
        scanf("%i", &comando);

        if (comando < 1 || comando > 4)
            continue;

        if (comando == 3)
        {
            printf("\nSelecione o sensor que deseja saber a medição\n"); // De 1 a 8?
            char msg[14];
            scanf("%i", &sensor);
            sprintf(msg, "Sensor %i", sensor);
            writeLCD(msg);
        }

        // ENVIO DO CODIGO DA REQUISICAO
        switch (comando)
        {
        case 1:
            // Envia o codigo da requisicao de situacao atual do sensor
            uart_tx(SITUACAO_ATUAL, 0, uart_filestream);
            break;

        case 2:
            // Envia o codigo da requisicao do valor da entrada analógica
            uart_tx(ENTRADA_ANALOGICA, 0, uart_filestream);
            break;
        case 3:
            // Envia o codigo da requisicao do valor de uma entrada digital
            uart_tx(SENSOR_DIGITAL, sensor, uart_filestream);
            break;
        case 4:
            // Envia o codigo da requisicao para acender/apagar o led
            uart_tx(CONTROLAR_LED, 0, uart_filestream);
            break;
        default:
            continue;
        }
        clearLCD();
        sleep(1); // Aguarda receber os dados.
        uart_rx();
    }

    close(uart_filestream);
    return 0;
}

void uart_tx(unsigned char com, unsigned char addr, int uart_filestream)
{
    printf("Tx");
    unsigned char tx_buffer[10];
    unsigned char *p_tx_buffer;

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = com;
    *p_tx_buffer++ = addr;

    if (uart_filestream != -1)
    {                                                                                    // Se abriu o arquivo da UART
        int cont = write(uart_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0])); // Filestream,mensagem enviada,tamanho da mensagem
        if (cont < 0)
            printf("Erro ao enviar os dados");
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
    }
}

//----------------------------------------------------------------------------------------------------------------------------------

unsigned char uart_rx()
{
    printf("entrou na função");
    unsigned char comandoResposta[100];
    int rx_length = read(uart_filestream, (void *)comandoResposta, 100);
    printf("rx_lengt: %i", rx_length);
    printf("tamanho do buffer: %d\n", rx_length);
    if (rx_length < 0)
    {
        printf("Erro no recebimento da resposta.");
    }
    else if (rx_length == 0)
    {
        printf("Nenhum dado disponível\n");
    }
    else
    {
        comandoResposta[rx_length] = '\0';
    }
    if (comandoResposta[0] == 0x00)
    {
        printf("Node MCU OK!");
        writeLCD("Node MCU OK!");
    }
    else if (comandoResposta[0] == 0x01) // Valor sensor analógico
    // Posição 0 - Tipo de resposta.
    {
        printf("\nValor da entrada\n");
        comandoResposta[0] = ' ';
        printf(comandoResposta); // Imprime valor da entrada analógica
        writeLCD("Analogico: ");
        writeLCD(comandoResposta);
    }
    else if (comandoResposta[0] == 0x02) // Valor sensor digital.
    {
        int dado = comandoResposta[1]; // Posição 1 - Dado
        printf("\nValor da entrada digital");
        printf("Sensor%i: %i", sensor, dado); // Imprime valor da entrada digital
        char msg[14];
        sprintf(msg, "Sensor %i: %i", sensor, dado);
        writeLCD(msg);
    }
    else if (comandoResposta[0] == 0x03)
    {
        printf("LED: ON");
        writeLCD("LED: ON");
    }
    else if (comandoResposta[0] == 0x04)
    {
        printf("LED: OFF");
        writeLCD("LED: OFF");
    }
    else
    {
        printf("NodeMCU Erro!");
        writeLCD("NodeMCU Erro!");
    }
}