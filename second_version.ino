#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// Pins
#define PIR_PIN 13
#define LIGHT_RELAY 26
#define FAN_RELAY 27

unsigned long lastMotionTime = 0;
unsigned long delayTime = 30000;

bool lightStatus = false;
bool fanStatus = false;

// Unique topic name (CHANGE this to something unique)
String baseTopic = "smartroom/natthanon123/";

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  String topicStr = String(topic);

  if (topicStr == baseTopic + "light") {
    if (message == "ON") {
      digitalWrite(LIGHT_RELAY, LOW);
      lightStatus = true;
    } else {
      digitalWrite(LIGHT_RELAY, HIGH);
      lightStatus = false;
    }
  }

  if (topicStr == baseTopic + "fan") {
    if (message == "ON") {
      digitalWrite(FAN_RELAY, LOW);
      fanStatus = true;
    } else {
      digitalWrite(FAN_RELAY, HIGH);
      fanStatus = false;
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_SmartRoom")) {
      client.subscribe((baseTopic + "light").c_str());
      client.subscribe((baseTopic + "fan").c_str());
    } else {
      delay(2000);
    }
  }
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);

  digitalWrite(LIGHT_RELAY, HIGH);
  digitalWrite(FAN_RELAY, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int motion = digitalRead(PIR_PIN);

  if (motion == HIGH) {
    digitalWrite(LIGHT_RELAY, LOW);
    digitalWrite(FAN_RELAY, LOW);
    lightStatus = true;
    fanStatus = true;
    lastMotionTime = millis();
  }

  if (lightStatus && (millis() - lastMotionTime > delayTime)) {
    digitalWrite(LIGHT_RELAY, HIGH);
    digitalWrite(FAN_RELAY, HIGH);
    lightStatus = false;
    fanStatus = false;
  }

  // Publish status every 2 seconds
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 2000) {
    client.publish((baseTopic + "status/light").c_str(), lightStatus ? "ON" : "OFF");
    client.publish((baseTopic + "status/fan").c_str(), fanStatus ? "ON" : "OFF");
    client.publish((baseTopic + "status/pir").c_str(), motion ? "MOTION" : "NO_MOTION");
    lastPublish = millis();
  }
}