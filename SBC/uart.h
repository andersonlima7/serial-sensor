    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <unistd.h>  //Usado para UART
    #include <fcntl.h>   //Usado para UART
    #include <termios.h> //Usado para UART

    int uart_filestream = -1;

    
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

    void uart_tx(unsigned char com, unsigned char addr)
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
        }
        else if (comandoResposta[0] == 0x01) // Valor sensor analógico
        // Posição 0 - Tipo de resposta.
        {
            printf("\nValor da entrada\n");
            comandoResposta[0] = ' ';
            printf(comandoResposta); 
        }
        else if (comandoResposta[0] == 0x02) // Valor sensor digital.
        {
            int dado = comandoResposta[1]; // Posição 1 - Dado
            printf("\nValor da entrada digital");
            // printf("Sensor%d: %d", sensor, dado); // Imprime valor da entrada digital
        }
        else if (comandoResposta[0] == 0x03)
        {
            printf("\n LED Ligado");
        }
        else if (comandoResposta[0] == 0x04)
        {
            printf("\n LED Desligado");
        }
        else
        {
            printf("\nO NodeMCU esta com problema");
        }
    }
