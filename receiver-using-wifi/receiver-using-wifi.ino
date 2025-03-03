#include <Arduino.h>
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

// PWM pins
////////////////////// CHANNEL CONFIGURATION //////////////////////////
#define channel_number 4  // Set the number of channels
const int sigPins[channel_number] = {25, 14, 26, 27 };  // Define output pins for each channel
///////////////////////////////////////////////////////////////////////

int pwm[channel_number];


// Struct for receiving joystick data
struct Data_receive {
  byte ch1; // throttle
  byte ch2; // yaw
  byte ch3; // pitch 
  byte ch4; // roll
  byte ch5; // aux 1
  byte ch6; // aux 2
};

typedef struct Data_receive Package;
Package dataReceived;

void setup() {
  Serial.begin(115200);


  setupWiFi();
  setupMQTT();

   setupPWM();
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
   setPWMValuesFromData();
  sendPWM();  // Generate PWM signals on the assigned pins
    Serial.print("Throttle: "); Serial.print(pwm[0]);
  Serial.print(" ||Yaw: "); Serial.print(pwm[1]);
  Serial.print(" ||Pitch: "); Serial.print(pwm[2]);
  Serial.print(" ||Roll: "); Serial.println(pwm[3]);
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
  mqttClient.setCallback(callback);
  reconnectMQTT();
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP32_Receiver")) {
      Serial.println("Connected to MQTT!");
      mqttClient.subscribe(mqttTopic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 3 seconds...");
      delay(3000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }

  // Parse the received data
  parseJoystickData(messageTemp);
}

void parseJoystickData(String data) {
  int index1 = data.indexOf(',');
  int index2 = data.indexOf(',', index1 + 1);
  int index3 = data.indexOf(',', index2 + 1);
  int index4 = data.indexOf(',', index3 + 1);
  int index5 = data.indexOf(',', index4 + 1);

  dataReceived.ch1 = data.substring(0, index1).toInt();
  dataReceived.ch2 = data.substring(index1 + 1, index2).toInt();
  dataReceived.ch3 = data.substring(index2 + 1, index3).toInt();
  dataReceived.ch4 = data.substring(index3 + 1, index4).toInt();
  dataReceived.ch5 = data.substring(index4 + 1, index5).toInt();
  dataReceived.ch6 = data.substring(index5 + 1).toInt();

}

void setupPWM() {
  for (int i = 0; i < channel_number; i++) {
    pinMode(sigPins[i], OUTPUT);
    digitalWrite(sigPins[i], LOW);
  }
}


void setPWMValuesFromData() {
  pwm[0] = constrain(map(dataReceived.ch1, 0, 255, 1000, 2000), 1000, 2000);
  pwm[1] = constrain(map(dataReceived.ch2, 0, 255, 1000, 2000), 1000, 2000);
  pwm[2] = constrain(map(dataReceived.ch3, 0, 255, 1000, 2000), 1000, 2000);
  pwm[3] = constrain(map(dataReceived.ch4, 0, 255, 1000, 2000), 1000, 2000);
}

void sendPWM() {
  for(int i = 0; i < channel_number; i++) {
    digitalWrite(sigPins[i], HIGH);
    delayMicroseconds(pwm[i]);
    digitalWrite(sigPins[i], LOW);
  }
}
