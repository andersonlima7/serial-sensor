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

//Variáveis que guardam o comando e o endereço do sensor na requisição.
byte command;
byte address;

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
  command = -1; // Comando requisitado
  address = -1; // Endereço requisitado
  digitalWrite(ledPin,HIGH);
  Serial.begin(9600);
}

void loop() {
  ArduinoOTA.handle();

  while(Serial.available() > 0) {
    command = Serial.read(); //Comando
    delay(500);
    address = Serial.read(); // Endereço do sensor

    switch(command){
      case 0x03: // Situação do NodeMCU
        command = 0x00;
        Serial.write(command);                      // NodeMCU funcionando OK.
        break;
      case 0x04: // Valor da entrada analógica.
        analogValue = analogRead(sensorAnalog);       // Ler o valor atual da entrada analógica. 
        command = 0x01;            //
        Serial.write(command);
        Serial.print(analogValue);
        break;
      case 0x05: // Valor da entrada digital.
      {
        int pino = address;                    // Valor do pino
        pinMode(pino, INPUT);                        // Define o pino como entrada.
        digitalValue = digitalRead(pino);            // Ler o valor do pino
        command = 0x02;
        Serial.write(command);
        Serial.write(digitalValue);
      }
        break;
      case 0x06: //Controla o LED, ligando se estiver desligado ou desligando se estiver ligado.
        digitalWrite(ledPin, !digitalRead(ledPin));
        if(digitalRead(ledPin)) // LED ligado
        {
          command = 0x03;
          Serial.write(command);
        }
        else {
          command = 0x04;
          Serial.write(command);
        }
        break;
      default:
          command = 0x1F;
          Serial.write(command);
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
