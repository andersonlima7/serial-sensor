#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  //Usado para UART
#include <fcntl.h>   //Usado para UART
#include <termios.h> //Usado para UART


void uart_tx(char *tx_string, int uart_filestream); // Funcao para envio de dados
void uart_rx(int uart_filestream, char *dado);      // Funcao para recebimento de dados

int main()
{
    int sensor = 0;                   // Armazena a opcao do sensor selecionado pelo usuário.
    int comando = 0;                  // Armazena a opcao de comando selecionado pelo usuÃ¡rio
    unsigned char comandoResposta[9]; // Armazena o comando de resposta lido pelo rx

    char atual = '3';     // codigo da situacao atual
    char analogico = '4'; // codigo do valor da entrada analogica
    char digital[3] = "5";   // codigo do valor da entrada digital
    char led = '6';       // codigo para acender/desligar o led

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Configuracao da UART

    // Abertura do arquivo da UART
    int uart_filestream = -1;
    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Abre em modo escrita/leitura bloqueado
    if (uart_filestream == -1)
    {
        printf("\nErro: nao eh posssivel abrir o arquivo da UART.\n");
    }
    // Struct para configuracao dos parametros de comunicacao
    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD; // Seta Baud-Rate para 115200, tamanho de 8 bits e sem paridade
    options.c_iflag = IGNPAR;                         // Ignora caracteres com erros de paridade
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH); // Libera entrada pendente. Esvazia a saida nao transmitida.
    tcsetattr(uart_filestream, TCSANOW, &options);

    //----------------------------------------------------------------------------------------------------------------------------------
    
   
    while (comando != -1)
    { 

    // Solicitacao de dados para criar a requisicao
    // Solicita ao usuario o tipo de informacao que deseja receber do sensor
    printf("\nSelecione a requisição que deseja realizar");
    printf("\n1 - Solicitar a situação atual do NodeMCU\n2 - Solicitar o valor da entrada analógica\n3 - Solicitar o valor de uma das entradas digitais.\n4 - Acender led da NodeMCU\n");
    scanf("%i", &comando);

    if(comando < 1 || comando > 4)
        continue;

    if (comando == 3){
        printf("\nSelecione o sensor que deseja saber a medição\n"); // De 1 a 8?
        scanf("%i", &sensor);
        digital[1] = sensor + '0'; // Salva o endereço do sensor na segunda posição do array.
    }

    // ENVIO DO CODIGO DA REQUISICAO
    switch (comando)
    {
    case 1:
        // Envia o codigo da requisicao de situacao atual do sensor
        uart_tx(atual, uart_filestream);
        break;
    case 2:
        // Envia o codigo da requisicao do valor da entrada analógica
        uart_tx(analogico, uart_filestream);
        break;
    case 3:
        // Envia o codigo da requisicao do valor de uma entrada digital
        uart_tx(digital, uart_filestream);
        break;
    case 4:
        // Envia o codigo da requisicao para acender o led
        uart_tx(led, uart_filestream);
        break;
    case 5:
        // Envia o codigo da requisicao para desligar o led
        uart_tx(ledOFF, uart_filestream);
        break;
    default:
        continue;
    }


    sleep(3); // Aguarda receber os dados.

    
    // Leitura do byte de codigo de resposta
    char dado[9];
    int rx_length;
    if (uart_filestream != -1)
    {
        rx_length = read(uart_filestream, (void *)comandoResposta, 8); // Filestream, buffer para armazenar, numero maximo de bytes lidos
        if (rx_length < 0)
        {
            printf("\nOcorreu um erro na leitura de dados");
        }
        else if (rx_length == 0)
        {
            printf("\nNenhum dado lido");
        }
        else
        {
            // Byte recebido
            comandoResposta[rx_length] = '\0';
            printf(comandoResposta);
            printf("%i", read(uart_filestream, (void *)comandoResposta, 8));
            if (strcmp(comandoResposta, "0x1F") == 0)
            {
                printf("\nO NodeMCU esta com problema");
                // writeLCD("NodeMCU: Erro");
            }
            else if (strcmp(comandoResposta, "0") == 0)
            {
                printf("\nO NodeMCU esta funcionando normalmente");
                // writeLCD("NodeMCU: OK");
            }
            else if (comandoResposta[0] == '1')  // Valor sensor analógico
            // Posição 0 - Tipo de resposta.
            {
                dado = comandoResposta[1];          // Posição 1 - Qtd de dígitos do dado.
                int tamanho = dado - '0';           // Converte o char para int.  
                int aux = dado + 1;
                char valorDado[aux];            // Armazena o valor da entrada analógica.
                int j = 0;
                for (int i = 2; i < tamanho+2; i+=1) { 
                    /*
                        Exemplo, valor de 1024, 4 bytes devem ser enviados, como a resposta a esse tipo 
                        de requisição é formada por 1 byte de tipo de resposta, 1 byte por quantidade de bytes
                        que devem ser recebidos, os dados da entrada analógica começa na posição 2 do array e 
                        vai até a quantidade de tamanho de bytes + 1, nesse exemplo, de dado[2] até dado[5].
                    */ 
                   valorDado[j] = comandoResposta[i];
                   j+=1;
                }
                
                printf("\nValor da entrada\n");
                printf(valorDado);                // Imprime valor da entrada analógica 
                // char msg[16] = "Analog:  ";
                // strcat(msg, valorDado);
                // writeLCD(msg);

            }
            else if (comandoResposta[0] == '2')  // Valor sensor digital.
            {
                dado = comandoResposta[1];      // Posição 1 - Dado 
                printf("\nValor da entrada digital");
                printf("%s ", dado);            // Imprime valor da entrada digital
                // int dado = comandoResposta[1] - '0';
                // char msg[14] = "Sensor : ";
                // msg[6] = sensor + '0'; // Sensor escolhido.
                // if(dado == 1){
                //     strcat(msg, "HIGH");
                // } 
                // else {
                //     strcat(msg, "LOW");
                // }
                // writeLCD(msg);
            }
            else if (comandoResposta, '3' == 0 ){
                printf("\n LED Ligado");
                // writeLCD("LED: ON");
            }
            else if (comandoResposta, '4' == 0 ){
                printf("\n LED Desligado");
                // writeLCD("LED: OFF");
            }
            else
            {
                printf("\nO NodeMCU esta com problema");
                // writeLCD("NodeMCU: Erro");
            }
        }
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
    }
    }
    close(uart_filestream);
    return 0;
}

void uart_tx(char *tx_string, int uart_filestream)
{
    if (uart_filestream != -1)
    {                                                         // Se abriu o arquivo da UART
        write(uart_filestream, tx_string, strlen(tx_string)); // Filestream,mensagem enviada,tamanho da mensagem
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
    }
}
//----------------------------------------------------------------------------------------------------------------------------------
// Funcao que recebe os dados de temperatura ou umidade
void uart_rx(int uart_filestream, char *dado)
{
    int rx_length;                // armazena o tamanho do dado lido
    unsigned char dadoInteiro[9]; // armazena a parte inteira do dado
    unsigned char dadoFracao[9];  // armazena a parte fracionaria do dado
    // leitura do byte de dado 1
    if (uart_filestream != -1)
    {
        rx_length = read(uart_filestream, (void *)dadoInteiro, 8); // Filestream, buffer para armazenar, nÃºmero maximo de bytes lidos
        if (rx_length < 0)
        {
            printf("\nOcorreu um erro na leitura de dados");
        }
        else if (rx_length == 0)
        {
            printf("\nNenhum dado lido");
        }
        else
        { // Se tem dado lido
            // Byte recebido
            dadoInteiro[rx_length] = '\0';
            // Copia o valor inteiro lido para a string dado
            strcpy(dado, dadoInteiro);
        }
        sleep(1);

        // leitura do byte de dado 2
        rx_length = read(uart_filestream, (void *)dadoFracao, 8); // Filestream, buffer para armazenar, numero maximo de bytes lidos
        if (rx_length < 0)
        {
            printf("\nOcorreu um erro na leitura de dados");
        }
        else if (rx_length == 0)
        {
            printf("\nNenhum dado lido");
        }
        else
        {
            // Byte recebido
            dadoFracao[rx_length] = '\0';
            // Concatena os valores da parte fracionaria com a inteira da medicao
            strcat(dado, dadoFracao);
        }
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
    }
}
