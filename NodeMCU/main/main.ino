#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <stdio.h>
#include <string.h>

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "ESP-10.0.0.109";

// Entradas e saídas

int ledPin = LED_BUILTIN; // Pino do LED.
const int sensorAnalog = A0; // Entrada analógica.

// Variáveis para guardar os valores dos sensores.
int analogValue = 0; // Valor lido da entrada analógica.
int digitalValue = 0; // Valor lido dos sensores digitais.


#define N_DIMMERS 3
int dimmer_pin[] = {14, 5, 15};
IPAddress local_ip 


void setup() {
  Serial.begin(115200); // Define a UART com um baud rate de 115200;

  pinMode(ledPin, OUTPUT); // Define o pino do LED como saída.
  // pinMode(sensor1, INPUT);  // Define os pinos dos sensores digitais como entrada.
  // pinMode(sensor2, INPUT); 
  // pinMode(sensor3, INPUT); 
  // pinMode(sensor4, INPUT); 
  // pinMode(sensor5, INPUT); 
  // pinMode(sensor6, INPUT); 
  // pinMode(sensor7, INPUT); 
  // pinMode(sensor8, INPUT); 
  
  //Mask 255.255.0.0

  // Configuração do WIFI
  Serial.println("Booting");
  WiFi.config(IPAddress local_ip, IPAddress gateway, IPAddress subnet)
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
  }
  /* switch off led */
  digitalWrite(ledPin, HIGH);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(ledPin, 990);

  for (int i = 0; i < N_DIMMERS; i++) {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i], 50);
  }

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
    for (int i = 0; i < N_DIMMERS; i++) {
      analogWrite(dimmer_pin[i], 0);
    }
    analogWrite(ledPin, 0);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    for (int i = 0; i < 30; i++) {
      analogWrite(ledPin, 127);
      delay(50);
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
}

void loop() {
  ArduinoOTA.handle();

  while(Serial.available() > 0) {
    char c1 = Serial.read(); //Comando
    delay(500);
    char c2 = Serial.read(); // Endereço do sensor

    switch(c1){
      case '3': // Situação do NodeMCU
        Serial.write("0");                      // NodeMCU funcionando OK.
        break;
      case '4': // Valor da entrada analógica.
        // Tipo de resposta - 1NDADO
        /**
         * N - Quantidade de dígitos do valor lido, pode ser de 1 a 4.
         * DADO - O número lido pela entrada analógica, de 0 a 1023.
        */
        analogValue = analogRead(sensorAnalog);       // Ler o valor atual da entrada analógica.
        int length = numberDigits(analogValue);
        char valueString[length + 1];                // n dígitos + 1 para o \O
        sprintf(valueString, "%ld", number);         // Int para string
        char lengthChar = length + '0'; 
        char message[length + 3];                      // Resposta a requisição da entrada analógica.
        message[0] = '1';
        message[1] = lengthChar;                    // Tamanho do valor da entrada analógica.
        strcat(message, valueString);               // Cria a mensagem de resposta.
        Serial.write(message, 7);
        break;
      case '5': // Valor da entrada digital.
        int pino = c2 - '0';                        // Valor do pino
        pinMode(pino, INPUT);                       // Define o pino como entrada.
        digitalValue = digitalRead(pino);           // Ler o valor do pino
        char valor = digitalValue + '0';
        char message[3] = "2";
        message[1] = valor; 
        Serial.write(message, 3);
        break;
      case '6': //Controla o LED, ligando se estiver desligado ou desligando se estiver ligado.
        digitalWrite(ledPin, !digitalRead(ledPin));
        if(digitalRead(ledPin)) // LED ligado
          Serial.write('3');
        else
          Serial.write('4');
        char message[3] = "3"
        break;
      default:
          break;
    }


    delay(1000);
  }
}

int numberDigits(int number) {
  if (number < 10) return 1;
  if (number < 100) return 2;
  if (number < 1000) return 3;
  if (number < 10000) return 4;
}
