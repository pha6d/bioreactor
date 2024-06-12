/*
 * ESP32 Code
 * 
 * This code receives data from the Arduino Mega via serial communication and sends it to a web server.
 * 
 * Connections:
 * - RX (pin 16) of ESP32 to TX of Arduino Mega
 * - TX (pin 17) of ESP32 to RX of Arduino Mega
 * - GND of ESP32 to GND of Arduino Mega
 * 
 * Libraries:
 * - WiFi: To handle WiFi connections
 * - HTTPClient: To handle HTTP requests
 * - NTPClient: To get the current time
 * - config.h: Contains the WiFi credentials
 * 
 * How it works:
 * - The ESP32 connects to the WiFi network.
 * - It waits for data from the Arduino Mega via Serial2.
 * - When data is received, it sends the data to a web server using an HTTP POST request.
 * - It sends a confirmation message back to the Arduino Mega.
 * 
 * Software Setup:
 * - Install the UART Driver (if needed):
 *   - Download and install the UART driver from Silabs USB to UART Bridge VCP Drivers.
 *   - URL: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads
 * 
 * - Install the ESP32 Board in Arduino IDE:
 *   - Open Arduino IDE.
 *   - Go to File > Preferences.
 *   - In the Additional Board Manager URLs field, add: https://dl.espressif.com/dl/package_esp32_index.json.
 *   - Go to Tools > Board > Boards Manager.
 *   - Search for ESP32 and install esp32 by Espressif Systems.
 * 
 * - Install Necessary Libraries:
 *   - Go to Sketch > Include Library > Manage Libraries.
 *   - Search for and install WiFi, HTTPClient, and NTPClient.
 */

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

// Define the pins for Serial2
const int rxPin = 16;
const int txPin = 17;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every 60 seconds

void setup() {
  // Initialize serial communication with the computer and the Arduino Mega
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, rxPin, txPin);
  Serial2.setTimeout(500); // Set timeout for Serial2

  Serial.println("ESP32 ready");
  delay(1000); // Time to stabilize

  // Connect to the WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP Client
  timeClient.begin();
}

void loop() {
  // Update time
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();

  // Wait for a message from the Arduino Mega
  if (Serial2.available()) {
    String arduinoMessage = Serial2.readStringUntil('\n');
    Serial.println("Value received from Arduino Mega: " + arduinoMessage);

    // Validate the JSON format
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, arduinoMessage);
    if (!error) {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://192.168.1.25:8000/sensor_data");  // Local IP of the Raspberry Pi
        http.addHeader("Content-Type", "application/json");  // Specify the content-type as JSON

        // Prepare JSON data
        String jsonData;
        serializeJson(doc, jsonData);
        jsonData = "{\"sensor_value\": " + jsonData + ", \"timestamp\": \"" + formattedTime + "\"}";
        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("Server response: " + response);
        } else {
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
        }
        http.end();
      }
    } else {
      Serial.println("Invalid JSON format received: " + arduinoMessage);
    }
  }
}
