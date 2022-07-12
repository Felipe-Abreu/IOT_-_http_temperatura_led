//Incluindo as bibliotecas necessárias
#include <SPI.h>
#include <Ethernet.h> //Necessária para a placa Ethernet
#include "ThingSpeak.h"

//Definindo variáveis do Thingspeak
String apiKey = "MR5T37P8QJTRTR30";   //Coloque a CHAVE do ThingSpeak entre as aspas
const char* server = "api.thingspeak.com";
unsigned long canal = 369586;         //Número do seu canal
String canal_temperatura = "1";            // canal da temperatura
int canal_led = 2;                    // canal do led


//variáveis do Projeto
#define TEMPO_ESPERA  20000           //tempo de atualização
#define LED 9
const int LM35 = A8; // Define o pino de leitura da saída do LM35


// Configurações para o Ethernet Shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x83, 0xEA }; // Entre com o valor do MAC
IPAddress ip(10,2,15,55); // Configure um IP válido 
byte gateway[] = { 10 , 2, 168, 1 }; //Entre com o IP do Computador onde a Câmera esta instalada
byte subnet[] = { 255, 255, 252, 0 }; //Entre com a Máskara de Subrede
EthernetClient client;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  delay(10);
  //configurando a ethernet via DHCP
  Serial.println("Aguardando DHCP...");
  if (Ethernet.begin(mac) == 0) {
    Serial.print("Falha ao Configurar DHCP...  ");
    Serial.println("Configurando manualmente");
    Ethernet.begin(mac, ip, gateway, subnet);
  }else{
    Serial.println("DHCP Configurado");
  }
  delay(1000);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  //Inicializando Cliente Thingspeak
  ThingSpeak.begin(client);
}

void loop() {
  escreve_temperatura();
  atua_led();
  delay(TEMPO_ESPERA);
}

void atua_led(){
  Serial.println("Atua!");
  //função de leitura do campo do Thingspeak
  float valor = ThingSpeak.readFloatField(canal, canal_led);

  Serial.print("Current status is: "); 
  Serial.println(valor);
  if(valor==1){
    digitalWrite(LED,HIGH);
    Serial.println("Led Ligado");
  }else if (valor==0){
    digitalWrite(LED,LOW);
    Serial.println("Led Desligado");
  }else{
    Serial.println("Valor Invalido");
  }
}

void escreve_temperatura(){
  Serial.println("Lendo temperatura!");
  //Lendo a temperatura
  float temperatura_m = (float(analogRead(LM35))*5/(1023))/0.01;
  //conectando ao servidor
  if (client.connect(server,80)) {
    String postStr = apiKey;
           postStr +="&field"+canal_temperatura+"=";
           postStr += String(temperatura_m);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     Serial.println(postStr);
     Serial.println("");
     Serial.print("Temperatura: ");        
     Serial.print(temperatura_m);                  
     Serial.println("C");
     Serial.println("Enviando para o ThingSpeak");
  }
  client.stop();                     
  Serial.println("Enviado!");
  Serial.println("");  
} 



