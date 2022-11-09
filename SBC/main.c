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

/**
 * Envia um comando e o endereco do sensor para o NodeMCU.
 * @param comando  Código do comando requisitado.
 * @param endereco  Endereço do sensor requisitado.
*/
void uart_tx(unsigned char comando, unsigned char endereco, int uart_filestream); 

/**
 * Recebe a resposta do NodeMCU.
 * @return Os dados recebidos do NodeMCU. 
*/
unsigned char uart_rx();                                                 

const int situacao_atual = 0x03    // código que requisita a situacao atual do NodeMCU.
const int entrada_analogica = 0x04 // código que requisita o valor da entrada analogica.
const int sensor_digital = 0x05    // código que requisita o valor da entrada digital.
const int controlar_led = 0x06     // código que requisita ligar/desligar o led.
int uart_filestream = -1;
int sensor = 0; // Armazena a opcao do sensor selecionado pelo usuário.

int main()
{
    mapMem();
    initLCD();
    writeLCD("Problema #2");
    // writeLCD("Comunicacao Serial");
    int comando = 0; // Armazena a opcao de comando selecionado pelo usuário.


    /**
     * Abertura do arquivo da UART
     * O_RDWR - Abre o arquivo para leitura e escrita.
     * O_NDELAY - Habilita o modo não bloqueante, isto é, tentativas de leitura ao arquivo podem retornar erro imediatamente
     * se não estiver disponível no momento, em vez de bloquear a aplicação.
    */
 
    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Abre em modo escrita/leitura bloqueado
    if (uart_filestream == -1)
    {
        printf("\nErro na abertura da UART.\n");
        writeLCD("Error:OPEN UART")
    }

    // 

    /**
     * Struct para configuração dos parâmetros de comunicação
     * Define a comunicação com um Baud-Rate para 9600, tamanho de 8 bits e sem paridade.
    */
    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; 
    options.c_iflag = IGNPAR;                       // Ignora caracteres com erros de paridade
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH); 
    tcsetattr(uart_filestream, TCSANOW, &options);

    // Interface com o usuário. 
    while (comando != -1)
    {
        // Solicitação de dados para criar a requisição ao NodeMCU.
        printf("\nSelecione a requisição que deseja realizar");
        printf("\n1 - Solicitar a situação atual do NodeMCU\n2 - Solicitar o valor da entrada analógica\n3 - Solicitar o valor de uma das entradas digitais.\n4 - Controlar led da NodeMCU\n");
        scanf("%i", &comando);
        sensor = 0; 

        if (comando < 1 || comando > 4)
            continue;

        if (comando == 3) //Comando da entrada digital.
        {
            printf("\nSelecione o sensor que deseja saber a medição\n"); // De 1 a 8?
            char msg[14];
            scanf("%i", &sensor);
            sprintf(msg, "Sensor %i", sensor);
            writeLCD(msg);
        }

        // Envia o código do comando requisitado para o NodeMCU.
        switch (comando)
        {
        case 1:
            // Envia o código da requisição de situação atual do sensor.
            uart_tx(situacao_atual, 0, uart_filestream);
            break;

        case 2:
            // Envia o código da requisição do valor da entrada analógica.
            uart_tx(entrada_analogica, 0, uart_filestream);
            break;
        case 3:
            // Envia o código da requisição do valor de uma entrada digital.
            uart_tx(sensor_digital, sensor, uart_filestream);
            break;
        case 4:
            // Envia o código da requisição para acender/apagar o led.
            uart_tx(controlar_led, 0, uart_filestream);
            break;
        default:
            continue;
        }
        clearLCD();
        sleep(2); // Aguarda receber os dados.

        unsigned char resposta[100] = uart_rx(); //Resposta do NodeMCU.

        if(strcmp(resposta, 'E') == 0) { // Erro
            continue;
        }

        switch (resposta[0]) // Comando de resposta.
        {
        case 0x00:
            printf("NodeMCU OK!");
            writeLCD("NodeMCU OK!");
            break;
        case 0x01:
            printf("\nValor da entrada\n");
            resposta[0] = ' ';  // Tira a primeira posição para não imprimir o comando na tela.
            printf(resposta);   // Imprime valor da entrada analógica
            writeLCD("Analogico: ");
            writeLCD(resposta);
            break;
        case 0x02:
            int dado = resposta[1]; // Posição 1 - Dado
            printf("\nValor da entrada digital");
            printf("Sensor%i: %i", sensor, dado); // Imprime valor da entrada digital
            char msg[14];
            sprintf(msg, "Sensor %i: %i", sensor, dado);
            writeLCD(msg);
            break;
        case 0x03:
            printf("LED: ON");
            writeLCD("LED: ON");
            break;
        case 0x04:
            printf("LED: OFF");
            writeLCD("LED: OFF");
            break;
        default: //0x1F
            printf("NodeMCU ERRO!");
            writeLCD("NodeMCU ERRO!");
            break;
        }
    }

    close(uart_filestream);
    return 0;
}

void uart_tx(unsigned char comando, unsigned char endereco, int uart_filestream)
{
    unsigned char tx_buffer[10]; // Array da mensagem a ser enviada.
    unsigned char *p_tx_buffer;     

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = comando;   // Primeiro define-se o comando da requisição.
    *p_tx_buffer++ = endereco;  // Depois, define-se o endereço do sensor requisitado.

    if (uart_filestream != -1)
    {   // Se abriu o arquivo da UART
        // Envia a mensagem.
        int count = write(uart_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0])); 
        if (count < 0) {
            printf("Erro ao enviar os dados");
            writeLCD("Erro: Envio";)
        }
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
        writeLCD("Error: OPEN FILE";)
    }
}

//----------------------------------------------------------------------------------------------------------------------------------

unsigned char uart_rx()
{
    unsigned char resposta[256];
    int rx_length = read(uart_filestream, (void *)resposta, 255);
    printf("Tamanho da resposta: %d\n", rx_length);
    if (rx_length < 0)
    {
        printf("Erro no recebimento da resposta.");
        writeLCD("Erro: Resposta");
        return 'E'; //Indica erro
    }
    else if (rx_length == 0)
    {
        printf("Nenhum dado disponível\n");
        writeLCD("Erro: dado vazio");
        return 'E'; //Indica erro
    }
    else
    {
        resposta[rx_length] = '\0';
    }

    return resposta;
}