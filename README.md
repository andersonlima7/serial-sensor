# Problema #2 – Interfaces de Entrada e Sáida

Este projeto tem por objetivo a construção de um sistema de comunicação entre uma Single Board Computer (SBC) e diversos sensores localizados em um dispositivo microcontrolador (Node MCU - ESP266), através do protocolo UART (<i>Universal asynchronous receiver/transmitter</i>).

## Sumário

- [Introdução](#introdução)
- [Objetivo](#objetivo)
- [Metolodogia](#metodologia)
- [Desenvolvimento](#desenvolvimento)
- [Conclusão](#conclusão)
- [Referências](#referências)

## Introdução

O ramo da Internet das Coisas (IoT) está em constante expansão, cada vez mais diversos equipamentos e produtos do dia a dia estão sendo inseridos em um ambiente de rede, por essa razão faz-se necessário entender como os equipamentos utilizados para propiciar a comunicação são programados e como inseri-lós no ambiente em rede, considerando a necessidade de integração com sistemas de controle, que normalmente são gerenciados por entes centrais, como SBC (Single Board Computer).

## Objetivo

Realizar a comunicação serial, via UART (Universal asynchronous receiver/transmitter), entre uma Single Board Computer (SBC) e o microcontrolador Node MCU (ESP8266) para enviar dados dos sensores conectados ao ESP8266 à SBC a partir das solicitações desta última, realizando a codificação de ambos os hardwares em linguagem C, bem como exibindo as respostas desta comunicação em um display LCD.

## Metodologia

Inicialmente os esforços foram direcionados a construção da estrutura necessária para que o SBC pudesse realizar a comunicação via UART, tanto quanto enviar os comandos (tx), fator essencial dado que todo o sistema de comunicação é controlado pela Raspberry, quanto receber as respostas às solicitações enviadas (rx), inicialmente testou-se a comunicação apenas da placa SBC implementando um loopback ao conectar o pino rx com o tx da própria raspberry. Ao finalizar a estrutura de comunicação da SBC, iniciou-se a construção dos mecanismos de captura dos valores dos sensores, bem como, a comunicação via UART no microcontrolador Node MCU. Após construir e testar separadamente a ESP8266, iniciou-se o processo de integração da comunicação entre o ente central, a SBC, e o Node MCU; os pinos RX e TX da SBC foram conectados aos pinos TX e RX do node MCU, o que permite uma conexão e comunicação entre ambos. Inicialmente houveram alguns gargalos de comunicação, sincronização e definição de valores a ser transmitido, no entanto esses elementos foram sendo ajustados a partir de testes e de algumas pesquisas sobre os elementos necessários.

## Desenvolvimento

### Microcontrolador

O microcontrolador consiste em um único circuito integrado que reúne um núcleo de processador, memórias voláteis e não voláteis e diversos periféricos de entrada e de saída de dados. Ou seja, ele nada mais é do que um computador muito pequeno capaz de realizar determinadas tarefas de maneira eficaz e sob um tamanho altamente compacto. A importância dos microcontroladores reside basicamente na alta demanda por sistemas embarcados do mercado atual. Isto é, sistemas computacionais compactos e de custo acessível que atendem a uma demanda específica. Como analogia, podemos observar o Arduino, por exemplo, que possui uma placa bastante completa e com diversos componentes que podem ser inúteis para determinados projetos. No problema aqui apresentado e resolve o microcontrolador consiste na Node MCU (ESP8266), que é uma plataforma Open Source criada para ser utilizado no desenvolvimento de projetos IoT (CARDOSO, 2020).

### UART

Na interface de comunicação serial, somente um bit de informação é transmitido/recebido por vez. Como os dados geralmente são processados em paralelo (por um microprocessador, por exemplo), há a necessidade de convertê-los em uma sequência de bits (e vice-versa). A maioria dos circuitos de comunicação serial utiliza um transceptor conhecido como UART, que é o acrônimo de Universal Asynchrounous Receiver/Transmiter ou Receptor/Transmissor Universal Assíncrono. Sua finalidade é possibilitar a transmissão e a recepção de dados originalmente disponíveis na forma paralela. O termo universal refere-se ao fato do formato do dado e velocidade serem configuráveis. Os níveis elétricos são delegados a circuitos especiais externos e não fazem parte da especificação da UART (BRAGA, 2010). A comunicação usada pela UART é assíncrona, ou seja, comandos precisam ser passados para sincronizar a comunicação. Geralmente um bit de start é enviado antes dos bits referentes à palavra serem enviados. Um bit de stop é enviado ao final para informar o término do envio. É comum haver um bit de paridade para checar por eventuais erros na transmissão. O tamanho da palavra a ser recebida/transmitida, assim como velocidade da transmissão, devem ser configurados e acordados entre receptor e transmissor. (BALDASSIN, 2019)

### Diagrama de Blocos

![Diagrama de Blocos](https://github.com/andersonlima7/serial-sensor/blob/develop/diagrama_bloco.png?raw=true)
Figura 1. Diagrama de Blocos que representa a solução desenvolvida no problema.

### SBC-Side

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

Na SBC há ainda uma especificidade, pois além de controlar as solicitações a serem realizadas a Node MCU, este possui a responsabilidade de controlar o Display LCD e exibir as saídas obtidas através das requisições, houveram algumas dificuldades de integração com o projeto desenvolvido anteriormente para o controle do Display (ver em: [Temporizador em um Display LCD](https://github.com/andersonlima7/timer)), especialmente dado o conflito de uso dos registradores da placa, tanto pelo display LCD (construído em `Assembly`) quanto pelo uso dos registradores pela forma estruturada pelo compilador o código a ser executado no processador.

### Microcontrolador-Side

Na NODE MCU há algumas especificidades ao lidarmos com a UART, por utilizarmos a linguagem do arduino conseguimos abstrair muitos elementos e, por exemplo, utilizar um objeto (o chamamos de `Serial`) e a partir dele realizar a configuração porta de comunicação serial (que realiza a comunicação em rede) e, por consequência, a UART ou USART (ARDUINO[a], 2019). A leitura e recebimento de dados via UART é feito através do método `read()` chamado através do objeto `Serial`, como mostram os códigos abaixo.

```ino
command = Serial.read(); //Comando
address = Serial.read(); // Endereço do sensor
```

O envio de dados para a SBC pode ocorrer através de dois métodos do objeto `Serial`, o método `write()` e o método `print()`. O método `Serial.write()`envia os os dados em binário para a porta serial, os dados são enviados em bytes ou séries de bytes. Já o método `Serial.print()` envia os dados para a porta serial no formato ASCII, em um formato que é humanamente legível, os bytes são enviados como um caracter único, os caracteres e strings são enviados como estes são devido a codificação em ASCII.

```c++
switch(command){
      case 0x03: // Situação do NodeMCU
        command = 0x00;
        Serial.write(command);                      // NodeMCU funcionando OK.
        break;
      case 0x04: // Valor da entrada analógica.
        analogValue = analogRead(sensorAnalog);       // Ler o valor atual da entrada analógica.
        command = 0x01;
        Serial.write(command);
        Serial.print(analogValue);                    //Envia os dados para a porta serial como um texto ASCII.
        break;
      case 0x05: // Valor da entrada digital.
      {
        int pino = address;                          // Pino que o sensor está pinado.
        pinMode(pino, INPUT);                        // Define o pino como entrada.
        digitalValue = digitalRead(pino);            // Ler o valor do sensor neste pino.
        command = 0x02;
        Serial.write(command);
        Serial.write(digitalValue);
      }
        break;
}
```

O microcontrolador ainda possui uma função essencial no problema que é a de capturar os dados de diversos sensores, sejam analógicos (nesse caso, a solução permite a realização da leitura de apenas um sensor analógioco) ou digitais (de 1 a 32 sensores digitais). Para a realização da leitura do sensor analógico utilizamos o método `analogRead`, que realiza a leitura de um valor de um pino especificado, a placa possui um conversor analógico-digital multicanal de 10 bits, isso significa que o conversor mapeará a voltagem entre 0 e a voltagem operacional (3.3V e 5V) e atribuirá, a partir disso, valores inteiros entre 0 e 1023. Para realizar a testagem do funcionamento deste sensor, utilizamos um potenciômetro, onde ao alterarmos o valor do mesmo percebemos as alterações nos valores e o funcionamento do conversor (ARDUINO[b], 2019). Já para a leitura dos sensores digitais utilizamos o método `digitalRead`, que realiza a leitura de um pino digital, capturando os valores sejam eles `HIGH` ou `LOW`, esse valor normalmente é expresso nos bits `0` ou `1`, a depender de como esteja pinado a placa (ARDUINO[c], 2019).

## Conclusão

Através desse projeto foi possível ter dimensão da importância das comunicações via Internet das Coisas, e a forma como esse é feito tanto a nível lógico baixo quanto em um nível de abstração maior, o que nos possibilitou também a compreensão da integração de códigos em Assembly (display LCD) e códigos em C (Comunicação UART). Este projeto também permite a compreensão do papel que os microcontroladores possuem na comunicação IoT, tanto a nível macro, quanto a nível específico e tecnicamente, dado que realizamos a configuração direta deste equipamento e o adaptamos ao problema apresentado. Por fim, percebeu-se também a forma como ocorre a comunicação Serial, através da configuração, tanto na SBC quanto na NodeMCU, do protocolo de comunicação assíncrono e serial UART. A solução aqui apresentada resolve o problema colocado, no entanto, é passível de diversas melhorias, a exemplo de um melhor controle das limitações de projeto e de eventuais tratamentos de erros que possam ocorrer por parte de usuário (ao selecionar pinos incorretos, por exemplo) ou não, pode-se ainda melhorar o tempo de realização da comunicação ao removermos alguns lapsos de tempo utilizadosno projeto.

## Referências

ARDUINO (a). Serial. 2019. Disponível em: <https://www.arduino.cc/reference/en/language/functions/communication/serial/>. Acesso em 10 Out 2022.

ARDUINO (b). analogRead(). 2019. Disponível em: <https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/>. Acesso em: 10 Out 2022.

ARDUINO (c). dialogRead(). 2019. Disponível em: <https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/>. Acesso em: 10 Out 2022.

BALDASSIN, Alexandre. Comunicação Serial (UART). 2019. Disponível em: <http://www1.rc.unesp.br/igce/demac/alex/disciplinas/MicroII/EMA864315-Serial.pdf>. Acesso 10 Out 2022.

BRAGA, NEWTON C. Como funcionam as UARTs (TEL006). 2010. Disponível em: <https://www.newtoncbraga.com.br/index.php/telecom-artigos/1709->. Acesso em 10 Out 2022.

CARDOSO, Matheus. O Que É Um Microcontrolador? 2020. Disponível em: <https://edu.ieee.org/br-ufcgras/o-que-e-um-microcontrolador/>. Acesso em 23 Set 2022.
