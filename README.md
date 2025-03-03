# ESP32 MQTT Joystick Controller

This project uses two ESP32 WROOM-32 modules to transmit and receive joystick data over Wi-Fi using MQTT.

## Features
- **Transmitter**: Reads joystick data and sends it via MQTT.
- **Receiver**: Receives data and converts it into PWM signals.

## Hardware Required
- ESP32 WROOM-32 (x2)
- Joysticks
- MQTT Broker (e.g., Mosquitto)

## Installation
1. Flash `transmitter-using-wifi/transmitter-using-wifi.ino` to the transmitting ESP32.
2. Flash `receiver-using-wifi/receiver-using-wifi.ino` to the receiving ESP32.
3. Set up an MQTT broker and configure the IP in the code.

## Usage
- Move the joystick, and the receiver will output PWM signals based on the movement.

### Author
**Tewodros (Teddy)**  
