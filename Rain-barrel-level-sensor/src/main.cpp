#include <Arduino.h>
#include <Ultrasonic.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define ULTRASONIC_TRIG D5
#define ULTRASONIC_ECHO D1
#define ULTRASONIC_TIMEOUT 40000UL
#define SERIAL_TIMEOUT 10000
// TODO vllt in EEPROM speichern
#define MQTT_BROKER_IP "192.168.178.42"
#define MQTT_BROKER_PORT 1883
#define MQTT_TOPIC "esp/water"

WiFiClient espClient;
PubSubClient client(espClient);

Ultrasonic ultrasound(ULTRASONIC_TRIG, ULTRASONIC_ECHO, ULTRASONIC_TIMEOUT);


void onMessage(char* topic, byte* payload, uint8_t length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char) payload[i];
  }
  
  if (message == "getWater") {
    Serial.print(message);
    uint8_t distance = ultrasound.read();
    String dist = String(distance);
    char const* pchar = dist.c_str();
    client.publish(MQTT_TOPIC, pchar);
    
    Serial.print("Distance: ");
    Serial.println(ultrasound.read());
  }

  Serial.println();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Port aus Ausgang schalten
  Serial.begin(9600); 
  Serial.setTimeout(SERIAL_TIMEOUT);

  Serial.println("SSID: "); 
  String ssid = Serial.readString();
  Serial.println("Password: ");
  String password = Serial.readString();

// TODO wahrscheinlich wird nur einmal WIFI_STA benötigt
  // Connect to Wifi
  //WiFi.mode(WIFI_STA); // Standard WiFi Client
  WiFi.begin(ssid, password);             // Connect to the network
  WiFi.mode(WIFI_STA); // Standard WiFi Client
 
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    Serial.print('.');
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(400);
  }

  Serial.println('\n'); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  Serial.print("Wifi SSID:\t");
  Serial.println(WiFi.SSID());

  digitalWrite(LED_BUILTIN, HIGH);

  
  // Connect to MQTT
  client.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);
  client.setCallback(onMessage);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
 // TODO in konsta
    if (client.connect("ESP8266Client", "test", "test" )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  client.subscribe("esp/water");

  Serial.println("Setup Done");
}

void loop() {
  client.loop();
  //digitalWrite(LED, LOW); //Led port ausschalten
  //delay(1000); //1 Sek Pause
  //digitalWrite(LED, HIGH); //Led port einschlaten
  //delay(1000);

  //Serial.print("Distance: ");
  //Serial.println(ultrasound.read());
  
  delay(5000);
}