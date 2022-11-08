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

![Diagrama de Blocos](https://github.com/andersonlima7/serial-sensor/blob/develop/diagrama.png?raw=true)

## Testes e Simulações

## Referências
