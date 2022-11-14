# Problema #2 – Interfaces de Entrada e Sáida

Este projeto tem por objetivo a construção de um sistema de comunicação entre uma Single Board Computer (SBC) e diversos sensores localizados em um dispositivo microcontrolador (Node MCU - ESP266), através do protocolo UART (<i>Universal asynchronous receiver/transmitter</i>).

## Sumário

- [Introdução](#introdução)
- [Objetivo](#objetivo)
- [Metolodogia](#metodologia)
- [Desenvolvimento](#desenvolvimento)
- [Testes e Simulações](#testes-e-simulações)
- [Referências](#referências)

## Introdução

O ramo da Internet das Coisas (IoT) está em constante expansão, cada vez mais diversos equipamentos e produtos do dia a dia estão sendo inseridos em um ambiente de rede, por essa razão faz-se necessário entender como os equipamentos utilizados para propiciar a comunicação são programados e como inseri-lós no ambiente em rede, considerando a necessidade de integração com sistemas de controle, que normalmente são gerenciados por entes centrais, como SBC (Single Board Computer).

## Objetivo

Realizar a comunicação serial, via UART (Universal asynchronous receiver/transmitter), entre uma Single Board Computer (SBC) e o microcontrolador Node MCU (ESP8266) para enviar dados dos sensores conectados ao ESP8266 à SBC a partir das solicitações desta última, realizando a codificação de ambos os hardwares em linguagem C, bem como exibindo as respostas desta comunicação em um display LCD.

## Metodologia

Inicialmente os esforços foram direcionados a construção da estrutura necessária para que o SBC pudesse realizar a comunicação via UART, tanto quanto enviar os comandos (tx), fator essencial dado que todo o sistema de comunicação é controlado pela Raspberry, quanto receber as respostas às solicitações enviadas (rx), inicialmente testou-se a comunicação apenas da placa SBC implementando um loopback ao conectar o pino rx com o tx da própria raspberry. Ao finalizar a estrutura de comunicação da SBC, iniciou-se a construção dos mecanismos de captura dos valores dos sensores, bem como, a comunicação via UART no microcontrolador Node MCU. Após construir e testar separadamente a ESP8266, iniciou-se o processo de integração da comunicação entre o ente central, a SBC, e o Node MCU; os pinos RX e TX da SBC foram conectados aos pinos TX e RX do node MCU, o que permite uma conexão e comunicação entre ambos. Inicialmente houveram alguns gargalos de comunicação, sincronização e definição de valores a ser transmitido, no entanto esses elementos foram sendo ajustados a partir de testes e de algumas pesquisas sobre os elementos necessários.

## Desenvolvimento

### Microcontrolador

O microcontrolador consiste em um único circuito integrado que reúne um núcleo de processador, memórias voláteis e não voláteis e diversos periféricos de entrada e de saída de dados. Ou seja, ele nada mais é do que um computador muito pequeno capaz de realizar determinadas tarefas de maneira eficaz e sob um tamanho altamente compacto. A importância dos microcontroladores reside basicamente na alta demanda por sistemas embarcados do mercado atual. Isto é, sistemas computacionais compactos e de custo acessível que atendem a uma demanda específica. Como analogia, podemos observar o Arduino, por exemplo, que possui uma placa bastante completa e com diversos componentes que podem ser inúteis para determinados projetos.

### UART

Na interface de comunicação serial, somente um bit de informação é transmitido/recebido por vez. Como os dados geralmente são processados em paralelo (por um microprocessador, por exemplo), há a necessidade de convertê-los em uma sequência de bits (e vice-versa). A maioria dos circuitos de comunicação serial utiliza um transceptor conhecido como UART, que é o acrônimo de Universal Asynchrounous Receiver/Transmiter ou Receptor/Transmissor Universal Assíncrono. Sua finalidade é possibilitar a transmissão e a recepção de dados originalmente disponíveis na forma paralela. O termo universal refere-se ao fato do formato do dado e velocidade serem configuráveis. Os níveis elétricos são delegados a circuitos especiais externos e não fazem parte da especificação da UART (BRAGA, 2010). A comunicação usada pela UART é assíncrona, ou seja, comandos precisam ser passados para sincronizar a comunicação. Geralmente um bit de start é enviado antes dos bits referentes à palavra serem enviados. Um bit de stop é enviado ao final para informar o término do envio. É comum haver um bit de paridade para checar por eventuais erros na transmissão. O tamanho da palavra a ser recebida/transmitida, assim como velocidade da transmissão, devem ser configurados e acordados entre receptor e transmissor. (BALDASSIN, 2019)

![Diagrama de Blocos](https://github.com/andersonlima7/serial-sensor/blob/develop/diagrama_bloco.png?raw=true)

A solução final do problema se deu de modo a, inicialmente, configurar o ambiente UART separadamente, tanto na Raspberry (SBC), quanto na Node MCU, definindo modo de abertura (já que uma das formas de controle da UART na SBC se dá de modo a ler e escrever arquivo onde a mesma tem seu driver localizado), baudrate (taxa de transmissão) e outros elementos necessários ao funcionamento da UART na Raspberry. Junto a isso fora configurado as funções responsáveis pelo envio e recebimento de informações em dois métodos `uart_tx` e `uart_rx`, respectivamente.

```c
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
            writeLCD("Erro: Envio");
        }
    }
    else
    {
        printf("\nFalha na abertura do arquivo");
        writeLCD("Error: OPEN FILE");
    }
}
```

```c
unsigned char uart_rx()
{
    int rx_length = read(uart_filestream, (void *)resposta, 255);
    printf("Tamanho da resposta: %d\n", rx_length);
    if (rx_length < 0)
    {
        printf("Erro no recebimento da resposta.");
        writeLCD("NodeMCU ERRO!");

    }
    else if (rx_length == 0)
    {
        printf("Nenhum dado disponível\n");
        writeLCD("NodeMCU ERRO!");
    }
    else
    {
        resposta[rx_length] = '\0';
    }

}
```

As funções mais enssenciais ao funcionamento dessa comunicação no método se expressam em `write`e `read`, ambas pertencem a biblioteca `unistd.h` e são responsáveis por fazer a escrita no arquivo `/dev/serial0` intermediador da comunicação de nível lógico baixo (o hardware da UART em si) e nível lógico alto, ao escrevemos no arquivo com o método `write` estamos colocando o que será enviado via UART, levando em consideração todas as limitações da tecnologia, em questão de tempo, taxa de transmissão e tamanho de blocos de envio de dados.

## Referências

BALDASSIN, Alexandre. Comunicação Serial (UART). 2019. Disponível em: <http://www1.rc.unesp.br/igce/demac/alex/disciplinas/MicroII/EMA864315-Serial.pdf>. Acesso 10 Out 2022.

BRAGA, NEWTON C. Como funcionam as UARTs (TEL006). 2010. Disponível em: <https://www.newtoncbraga.com.br/index.php/telecom-artigos/1709->. Acesso em 10 Out 2022.

CARDOSO, Matheus. O Que É Um Microcontrolador? 2020. Disponível em: <https://edu.ieee.org/br-ufcgras/o-que-e-um-microcontrolador/>. Acesso em 23 Set 2022.
