#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DallasTemperature.h>
#include "uptime.h"

float TempHigh = 17;
float TempLow = 15;
float temperatureC = 0;
const int pinoRele = 5;
const int oneWireBus = 2;   
const int mqttPort = 18144;
const char* mqttUser = "xiifwdfp";
const char* mqttPassword = "BOoDNVIMchWg";
const char* ssid = "JDHOUSE";
const char* password =  "@j&dvds&2019";
const char* mqttServer = "soldier.cloudmqtt.com";
void recebePacote(char* topic, byte* payload, unsigned int length);


WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
  WiFi.begin(ssid, password);
  }
}

void conecta_mqtt() {
 setup_wifi();
 if (!client.connected()) {
 client.connect("ESP8266Client", mqttUser, mqttPassword );
 client.subscribe("TempMax");
 client.subscribe("TempMin");
 }
}

void setup() {
      
  Serial.begin(9600);
  sensors.begin();
  setup_wifi();
  pinMode(LED_BUILTIN, OUTPUT);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String payloadStr ="";
  for (int i = 0; i < length; i++) {
   payloadStr += (char)payload[i];
  }
  Serial.println();
  if (String(topic).equals("TempMax")){
  char floatbuf[32];
  payloadStr.toCharArray(floatbuf, sizeof(floatbuf));
  TempHigh = atof(floatbuf);
  Serial.print("Recebido  - ");
  Serial.print(TempHigh);
  Serial.println();
 }
  if (String(topic).equals("TempMin")){
  char floatbuf[32];
  payloadStr.toCharArray(floatbuf, sizeof(floatbuf));
  TempLow = atof(floatbuf);
  Serial.print("Recebido  - ");
  Serial.print(TempLow);
  Serial.println();
 }
}

void getuptime() {
   uptime::calculateUptime();
  
  
  Serial.print(uptime::getDays());
  Serial.print(" Dias   ");
    
  Serial.print(uptime::getHours());
  Serial.print(":");
  
  Serial.print(uptime::getMinutes());
  Serial.print(":");
  
  Serial.print(uptime::getSeconds());
  Serial.print("s");
   
  Serial.print("\n");
  }

void sendinfos() {
  conecta_mqtt();
  client.publish("TermFer", String(temperatureC).c_str(),true);
  client.publish("CurrentMaxTemp", String(TempHigh).c_str(),true);
  client.publish("CurrentLowTemp", String(TempLow).c_str(),true);
  Serial.print("temperatura maxima atual - ");
  Serial.println(TempHigh);
  Serial.print("temperatura Minima atual - ");
  Serial.println(TempLow);
  getuptime();
}

void Set(){
  pinMode(5, OUTPUT);
  sensors.requestTemperatures(); 
  temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");  
  }
  
void loop() {
  pinMode(5, OUTPUT);
  sensors.requestTemperatures(); 
  temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  client.loop();  
  
  while (temperatureC < TempHigh) {
  Set();
  client.loop();
  digitalWrite(pinoRele,LOW);
  conecta_mqtt();
  client.publish("MotorState", "0");
  sendinfos();
  
  delay(10000);
  }

Serial.print ("Trocando para o loop Ligado");
Serial.println ();

  while (temperatureC > TempLow ){
  Set();
  client.loop();
  digitalWrite(pinoRele,HIGH);
  conecta_mqtt();
  client.publish("MotorState", "1");
  sendinfos();
  delay(10000);
  }
  
Serial.print ("Trocando para o loop Desligado");
Serial.println ();
  
}
