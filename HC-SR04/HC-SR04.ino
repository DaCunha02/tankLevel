#include <Ultrasonic.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

Ultrasonic leitura(8,7); // define o nome do sensor(leitura)
//e onde esta ligado o trig(8) e o echo(7) respectivamente

long distancia;
int base = 625; //define o valor em cm^2 da base
float volume;
float a;

const char* ssid = "Guicunha";  
const char* password = "cestaobom";

const char* mqtt_server = "18.116.38.84";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("Tanque");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}
void loop() {

  distancia = leitura.Ranging(CM); //retorna a distancia em cm
  Serial.print(distancia); //imprime o valor da variavel distancia
  Serial.println("cm");

  volume = distancia * base; //define o volume presente no tanque em cm^3
  a = volume/1000; //converte cm^3 para litro

  Serial.print(volume); //imprime o valor da variavel volume
  Serial.println("cm^3");

  Serial.print(a); //imprime o valor da variavel nivel
  Serial.println("Litros");
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
    float nivel = a;
    // Convert the value to a char array
    char levelString[8];
    dtostrf(nivel, 1, 2, levelString);
    Serial.print("nivel: ");
    Serial.println(levelString);
    client.publish("Tanque", levelString);
  } 
