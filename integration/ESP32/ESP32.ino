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
 *   - Connect your ESP32
 *   - In Device Manager, find the unrecognized device or one with an exclamation mark under Ports (COM & LPT) or Other Devices.
 *   - Right-click the device, select Update driver, then choose Browse my computer for drivers.
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
 *
 * - Select the board "ESP32 Dev Module"
 */

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

// Define the pins for Serial2
const int rxPin = 18;
const int txPin = 19;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every 60 seconds

void setup() {
  Serial.begin(115200); // Initialize the serial communication for debugging at a baud rate of 115200
  delay(3000); // Delay for 3 seconds to allow the serial connection to stabilize
  Serial.println("ESP32 Ready"); // Print a message indicating the ESP32 is ready
  Serial2.begin(9600, SERIAL_8N1, rxPin, txPin); // Initialize the serial communication for communication with the Arduino Mega at a baud rate of 9600
  Serial2.setRxBufferSize(256); // Increase the size of the receive buffer to 256 bytes
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
  static String receivedData = ""; // Create a static string to store incoming data

  while (Serial2.available()) { // Check if data is available to read from Serial2
    char incomingChar = Serial2.read(); // Read one character from Serial2
    receivedData += incomingChar; // Append the incoming character to the receivedData string

    if (incomingChar == '\n') { // If the incoming character is a newline, it means the end of a message
      Serial.print("Received from Arduino Mega: "); // Print the prefix for the received message
      Serial.println(receivedData); // Print the received message

      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, receivedData);
      if (!error) {
        Serial.println("Valid JSON received"); // Indicate valid JSON was received
        Serial.println(receivedData); // Print the valid JSON data for debugging

        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin("http://192.168.1.25:8000/sensor_data");  // Local IP of the Raspberry Pi
          http.addHeader("Content-Type", "application/json");  // Specify the content-type as JSON

          // Prepare JSON data
          if (doc.containsKey("ev") && doc["ev"] == "startup") {
            doc["event"] = doc["ev"];
            doc["programType"] = doc["pt"];
            doc["rateOrSpeed"] = doc["rate"];
            doc["duration"] = doc["dur"];
            doc["tempSetpoint"] = doc["tSet"];
            doc["phSetpoint"] = doc["phSet"];
            doc["doSetpoint"] = doc["doSet"];
            doc["nutrientConc"] = doc["nutC"];
            doc["baseConc"] = doc["baseC"];
            doc["experimentName"] = doc["expN"];
            doc["comment"] = doc["comm"];
          } else {
            doc["event"] = "data";
            doc["programType"] = doc["prog"];
            doc["rateOrSpeed"] = 0;
            doc["duration"] = 0;
            doc["tempSetpoint"] = 0.0;
            doc["phSetpoint"] = 0.0;
            doc["doSetpoint"] = 0.0;
            doc["nutrientConc"] = 0.0;
            doc["baseConc"] = 0.0;
            doc["experimentName"] = "";
            doc["comment"] = "";
          }

          doc["currentProgram"] = doc["prog"];
          doc["programStatus"] = doc["stat"];
          doc["airPumpStatus"] = doc["ap"];
          doc["drainPumpStatus"] = doc["dp"];
          doc["nutrientPumpStatus"] = doc["np"];
          doc["basePumpStatus"] = doc["bp"];
          doc["stirringMotorStatus"] = doc["sm"];
          doc["heatingPlateStatus"] = doc["hp"];
          doc["ledGrowLightStatus"] = doc["lg"];
          doc["waterTemp"] = doc["wT"];
          doc["airTemp"] = doc["aT"];
          doc["ph"] = doc["pH"];
          doc["turbidity"] = doc["tb"];
          doc["oxygen"] = doc["ox"];
          doc["airFlow"] = doc["af"];

          String jsonData;
          serializeJson(doc, jsonData);
          jsonData = "{\"sensor_value\": " + jsonData + ", \"timestamp\": \"" + timeClient.getFormattedTime() + "\"}";
          Serial.print("Sending JSON to server: "); // Indicate the JSON being sent
          Serial.println(jsonData); // Print the JSON data being sent for debugging

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
        Serial.println("Invalid JSON format received: " + receivedData);
      }
      receivedData = ""; // Reset the receivedData string for the next message
    }
  }
  delay(500); // Wait for 500 milliseconds

  // Check buffer usage
  int bufferSize = Serial2.availableForWrite(); // Get the available space in the write buffer
  int percentageUsed = (100 * (256 - bufferSize)) / 256; // Calculate the percentage of the buffer used
  Serial.print("Buffer used: "); // Print the prefix for the buffer usage
  Serial.print(percentageUsed); // Print the buffer usage percentage
  Serial.println("%"); // Print the percentage symbol
}
