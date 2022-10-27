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
    int sensor = 0;                   // Armazena a opcao do sensor selecionado pelo usuÃ¡rio
    int comando = 0;                  // Armazena a opcao de comando selecionado pelo usuÃ¡rio
    unsigned char comandoResposta[9]; // Armazena o comando de resposta lido pelo rx

    char atual[] = "0x03";     // codigo da situacao atual
    char analogico[] = "0x04"; // codigo do valor da entrada analogica
    char digital[] = "0x05";   // codigo do valor da entrada digital
    char led[] = "0x06";       // codigo para acender o led

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
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD; // Seta Baud-Rate para 9600, tamanho de 8 bits e sem paridade
    options.c_iflag = IGNPAR;                         // Ignora caracteres com erros de paridade
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH); // Libera entrada pendente. Esvazia a saida nao transmitida.
    tcsetattr(uart_filestream, TCSANOW, &options);

    // Solicita ao usuario o sensor que deseja obter informacoes
    printf("\nInforme o valor do sensor do qual deseja obter informações");
    scanf("%i", &sensor);
    // Tratar valor

    //----------------------------------------------------------------------------------------------------------------------------------
    // Solicitacao de dados para criar a requisicao
    // Solicita ao usuario o tipo de informacao que deseja receber do sensor
    printf("\nSelecione a requisição que deseja realizar");
    printf("\n1 - Solicitar a situação atual do NodeMCU\n2 - Solicitar o valor da entrada analógica\n3 - Solicitar o valor de uma das entradas digitais.\n4 - Acender led da NodeMCU\n");
    scanf("%i", &comando);
    while (comando < 1 || comando > 4)
    { // Solicita novamente ate que a opcao seja valida.
        printf("\nInformacao invalida, selecione uma disponivel:");
        printf("\n1 - Solicitar a situação atual do NodeMCU\n2 - Solicitar o valor da entrada analógica\n3 - Solicitar o valor de uma das entradas digitais.\n4 - Acender led da NodeMCU\n");
        scanf("%i", &comando);
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
    default:
        printf("\n\nInformacao invalida, selecione uma disponivel:");
        printf("\n1 - Solicitar a situação atual do NodeMCU\n2 - Solicitar o valor da entrada analógica\n 3 - Solicitar o valor de uma das entradas digitais.\n4 - Acender led da NodeMCU\n");
        scanf("%i", &comando);
    }

    printf("\nRequisicao sendo enviada. Aguarde uns instantes");
    char valorSensor[] = "";
    sprintf(valorSensor, "0x0%i", sensor);
    uart_tx(valorSensor, uart_filestream);
    printf("valor sensor é: %s", valorSensor);

    // ENVIO DO ENDERECO DA REQUISICAO
    /*switch (sensor)
    {
    case 1:
        // Envia o codigo de endereco do sensor
        break;
    default:
        printf("\n\n Opcao de sensor invalida. Selecione o sensor corretamente");
        printf("\n1 - Sensor\n");
        scanf("%i", &sensor);
    }*/
    sleep(3);

    /*int repeticaoMenu = 0;
    while (repeticaoMenu == 0)
    {

        // Solicita ao usuario o sensor que deseja obter informacoes
        printf("\nSelecione o sensor que deseja receber informacoes");
        printf("\n1 - DHT11\n");
        scanf("%i", &sensor);
        while (sensor != 1)
        { // Solicita novamente ate que a opcao seja valida.
            printf("\nOpcao de sensor invalida. Selecione o sensor corretamente");
            printf("\n1 - DHT11\n");
            scanf("%i", &sensor);
        }
    */

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
            // printf(comandoResposta);
            printf("%i", read(uart_filestream, (void *)comandoResposta, 8));
            if (strcmp(comandoResposta, "0x1F") == 0)
            {
                printf("\nO sensor esta com problema");
            }
            else if (strcmp(comandoResposta, "0x00") == 0)
            {
                printf("\nO sensor esta funcionando normalmente");
            }
            else if (strcmp(comandoResposta, "0x01") == 0)
            {
                printf("\nValor da entrada analógica");
                uart_rx(uart_filestream, dado); // Recebe o valor da entrada analógica
                printf("%s ", dado);            // Imprime valor da entrada analógica
            }
            else if (strcmp(comandoResposta, "0x02") == 0)
            {
                printf("\nValor da entrada digital");
                uart_rx(uart_filestream, dado); // Recebe o valor entrada digital
                printf("%s ", dado);            // Imprime valor da entrada digital
            }
            else
            {
                printf("\n\nErro no formato de dado recebido");
            }
        }
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
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
