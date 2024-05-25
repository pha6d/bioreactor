#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"

/*
 * ESP-12F WiFi Module Configuration
 * 
 * We are using the ESP-12F NodeMCU module to connect to WiFi.
 *
 * ESP8266WiFi Library:
 * This library is used for connecting the ESP8266 to WiFi networks.
 * 
 * Install the CH340 Driver:
 * 1. Download and install the CH340 driver from https://sparks.gogo.co.nz/ch340.html
 *
 * Install the ESP8266 Board Package:
 * 1. Open Arduino IDE.
 * 2. Go to File > Preferences.
 * 3. In "Additional Board Manager URLs" field, add: http://arduino.esp8266.com/stable/package_esp8266com_index.json
 * 4. Go to Tools > Board > Boards Manager.
 * 5. Search for "ESP8266" and install "ESP8266 by ESP8266 Community".
 * 6. Go to Sketch > Include Library > Manage Libraries.
 * 7. Search for "ESP8266WiFi" and install it.
 *
 * Connect to the board: ESP8266 > NodeMCU 1.0 (ESP-12E)
 * 
 * 
 * Upload the code below directly into the ESP-12F module via USB..
 * 
 * Then connect ESP-12F to Arduino Mega:
 * 1. VCC to 3.3V
 * 2. GND to GND
 * 3. TX from ESP to RX from Arduino Mega
 * 4. RX from ESP to TX from Arduino Mega
 * 5. EN (CH_PD) to 3.3V 
 * 
 * Reboot and Flash:
 * - The module can be rebooted with the 'RST' button.
 * - The 'FLASH' button is used to put the ESP in flash mode to upload code, but is not necessary for our current use.
 *
 * WiFi credentials:
 * WiFi connection information is in the "config.h" file.
 */

WiFiClientSecure client;

void setup() {
    Serial.begin(115200); // Start serial communication at 115200 baud for debugging
    Serial1.begin(115200); // Communication with Arduino Mega
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid); // Debugging message

    WiFi.begin(ssid, password); // Begin WiFi connection

    // Wait for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("."); // Print dots while connecting
        attempts++;
        if (attempts > 20) { // Timeout after 10 seconds (20 attempts * 0.5 seconds)
            Serial.println("Failed to connect to WiFi");
            return;
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); // Print the local IP address
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        // Check if data is available from Arduino Mega
        if (Serial1.available()) {
            String payload = Serial1.readStringUntil('\n');

            // Debug: Print the payload received from Arduino Mega
            Serial.println("Received from Arduino Mega: " + payload);

            client.setInsecure();  // Not the best practice, but allows for testing without certificate validation

            if (!client.connect(server_host, server_port)) {
                Serial.println("Connection to server failed");
                return;
            }

            // Send HTTP POST request
            client.println("POST " + String(server_path) + " HTTP/1.1");
            client.println("Host: " + String(server_host));
            client.println("Content-Type: application/x-www-form-urlencoded");
            client.print("Content-Length: ");
            client.println(payload.length());
            client.println();
            client.print(payload);

            // Wait for server response
            unsigned long start = millis();
            while (client.connected() && !client.available() && millis() - start < 2000) delay(1);
            while (client.available()) {
                String line = client.readStringUntil('\n');
                Serial.println("Server: " + line);
            }

            client.stop();
        }
    }

    delay(1000); // Send data every second
}
