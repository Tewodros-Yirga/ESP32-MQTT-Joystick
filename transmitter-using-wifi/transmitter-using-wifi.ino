#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "DTU-STAFFS";
const char* password = "";

// MQTT Broker details
const char* mqttServer = "10.1.198.39"; // Replace with your local IP
const int mqttPort = 1883;
const char* mqttTopic = "quadcopter/joystick";

// MQTT and Wi-Fi Client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

struct Data_transmit {
  byte ch1; // throttle
  byte ch2; // yaw
  byte ch3; // pitch 
  byte ch4; // roll
  byte ch5; // aux 1
  byte ch6; // aux 2
};

typedef struct Data_transmit Package;
Package dataTosend;

void setup() {
  Serial.begin(115200);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(36, INPUT);
  pinMode(39, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);

  setupWiFi();
  setupMQTT();
}

void loop() {
  dataTosend.ch1 = mapJoystickValues(analogRead(33), 0, 2047, 4095, false);
  dataTosend.ch2 = mapJoystickValues(analogRead(32), 0, 2047, 4095, false);
  dataTosend.ch3 = mapJoystickValues(analogRead(39), 0, 2047, 4095, false);
  dataTosend.ch4 = mapJoystickValues(analogRead(36), 0, 2047, 4095, false);
  dataTosend.ch5 = mapJoystickValues(analogRead(35), 0, 2047, 4095, false);
  dataTosend.ch6 = mapJoystickValues(analogRead(34), 0, 2047, 4095, false);

  publishJoystickData();
  delay(10);
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle)
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return (reverse ? 255 - val : val);
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP32_Transmitter")) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 3 seconds...");
      delay(3000);
    }
  }
}

void publishJoystickData() {
  if (mqttClient.connected()) {
    String joystickData = String(dataTosend.ch1) + "," + String(dataTosend.ch2) + "," + String(dataTosend.ch3) + "," + String(dataTosend.ch4) + "," + String(dataTosend.ch5) + "," + String(dataTosend.ch6);
    mqttClient.publish(mqttTopic, joystickData.c_str());
    Serial.println("Joystick Data published to MQTT:");
    Serial.println(joystickData);
  } else {
    Serial.println("MQTT disconnected. Reconnecting...");
    setupMQTT();
  }
}