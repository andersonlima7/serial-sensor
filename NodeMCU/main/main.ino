#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "ESP-10.0.0.109";

int led_pin = LED_BUILTIN;
#define N_DIMMERS 3
int dimmer_pin[] = {14, 5, 15};

void setup() {
  Serial.begin(115200);

  /* switch on led */
  pinMode(led_pin, OUTPUT);
  

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
  }
  /* switch off led */
  digitalWrite(led_pin, HIGH);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(led_pin, 990);

  for (int i = 0; i < N_DIMMERS; i++) {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i], 50);
  }

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
    for (int i = 0; i < N_DIMMERS; i++) {
      analogWrite(dimmer_pin[i], 0);
    }
    analogWrite(led_pin, 0);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    for (int i = 0; i < 30; i++) {
      analogWrite(led_pin, 127);
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
    char c2 = Serial.read();

    switch(c1){
      case '3':
        Serial.write("0");
        break;
      case '4':
        Serial.write("1");
        break;
      case '5':
        Serial.write("2");
        break;
      case '6':
        Serial.write("LED");
        digitalWrite(led_pin, LOW); //Acende o led
        break;
    }


    delay(1000);
  }
}
