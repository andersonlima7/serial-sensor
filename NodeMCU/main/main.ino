#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif


// Definições de rede
IPAddress local_IP(10, 0, 0, 109);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

// Nome do ESP na rede
const char* host = "ESP-10.0.0.109";


const char* ssid = STASSID;
const char* password = STAPSK;

int ledPin = LED_BUILTIN; // Pino do LED.
const int sensorAnalog = A0; // Entrada analógica.

// Variáveis para guardar os valores dos sensores.
int analogValue = 0; // Valor lido da entrada analógica.
int digitalValue = 0; // Valor lido dos sensores digitais.



void code_uploaded(){
  for(int i=0;i<2;i++){
    digitalWrite(LED_BUILTIN,LOW);
    delay(150);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(150);
  }
}

void OTA_setup(){
  
  Serial.begin(115200);
  Serial.println("Booting");

  // Configuração do IP fixo no roteador, se não conectado, imprime mensagem de falha
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}


void setup() {
  code_uploaded();
  OTA_setup(); 
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,HIGH);
  Serial.begin(9600);
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
