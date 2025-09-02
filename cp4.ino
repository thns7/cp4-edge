/*
  CP 4 - Edge Computing
  
  Autores: 
  Enzo Ozzioli
  Thiago Nascimento 
  Vinicius Herreira
  Gustavo Rocha
  Gabriel Santos
*/

#include <WiFi.h> // Inclue a biblioteca Wifi
#include "DHT.h" // Inclue a biblioteca DHT
#include "ThingSpeak.h" // Inclue a biblioteca ThingSpeak

const char* ssid = "Wokwi-GUEST"; // Nome do WIFI
const char* password = ""; // Senha do Wifi

unsigned long channelID = 3058696; // Channel ID do Channel
const char* writeAPIKey = "W5M5CDKNR2LZY2SM"; // Write API do Channel

#define DHTPIN 15 // Pino ultilizado no ESP32 para ler o DHT
#define DHTTYPE DHT22 // Tipo de DHT ultilizado (no wokmi só possui o 22 porém ultilizamos o 11)
#define LDR_PIN 34 // Pino ultilizado no ESP32 para ler o LDR

// Criação do DHT ultilizando o pino e o tipo definido
DHT dht(DHTPIN, DHTTYPE);

// cliente WiFi pra biblioteca ThingSpeak usar
WiFiClient client;

void setup() {
  Serial.begin(115200);  // Monitor serial começa em 115200 bps
  WiFi.begin(ssid, password); // Começa a conexão WiFi

  // Enquanto não conectar, fica mostrando mensagem a cada 1 segundo
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("WiFi conectado!"); // Quando conectar, mostra essa mensagem
  
  ThingSpeak.begin(client); // Inicializa a comunicação com o ThingSpeak
  dht.begin(); // Inicializa o sensor DHT
}

void loop() {
  // Faz a leitura dos sensores
  float temperatura = dht.readTemperature(); // Lê a temperatura (°C)
  float umidade = dht.readHumidity(); // Lê a Umidade (%)
  int luminosidade = analogRead(LDR_PIN); // Lê a luminosidade (0 a 4095 no ESP32)

  // Se a leitura do DHT falhar, mostra erro e sai da função
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Erro na leitura do DHT11");
    return;
  }

  // Mostra no monitor serial os valores lidos
  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" °C | Umidade: ");
  Serial.print(umidade);
  Serial.print(" % | Luminosidade: ");
  Serial.println(luminosidade);

  // Coloca os valores nos campos do ThingSpeak
  ThingSpeak.setField(1, temperatura); // Campo 1 = Temperatura
  ThingSpeak.setField(2, umidade); // Campo 2 = Umidade
  ThingSpeak.setField(3, luminosidade); // Campo 3 = Luminosidade

   // Envia os dados pro canal do ThingSpeak
  int statusCode = ThingSpeak.writeFields(channelID, writeAPIKey);

  // Se deu certo (código 200), mostra mensagem de sucesso
  if (statusCode == 200) {
    Serial.println("Dados enviados com sucesso!");
  } else {
    // Se não, mostra o código do erro
    Serial.print("Erro no envio: ");
    Serial.println(statusCode);
  }

  delay(20000);
}