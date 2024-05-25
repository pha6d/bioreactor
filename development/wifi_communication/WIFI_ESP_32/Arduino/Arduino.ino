/*
 * Arduino Uno Code
 * 
 * This code generates a random number and sends it to the ESP32 via serial communication.
 * The ESP32 will then send this data to a web server.
 * 
 * Connections:
 * - RX (pin 10) of Arduino Uno to TX of ESP32
 * - TX (pin 11) of Arduino Uno to RX of ESP32
 * - GND of Arduino Uno to GND of ESP32
 * 
 * Libraries:
 * - SoftwareSerial: To create additional serial ports
 * - TimeLib: To handle time-related functions
 * 
 * How it works:
 * - The Arduino Uno generates a random number every second.
 * - It sends the random number to the ESP32 via SoftwareSerial.
 * - It waits for a confirmation message from the ESP32.
 * - If the confirmation is received, it prints a success message. Otherwise, it prints an error message.
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
 *   - Search for and install WiFi and HTTPClient.
 *   - For the Arduino Uno, search for and install SoftwareSerial and Time.
 */

#include <SoftwareSerial.h>
#include <TimeLib.h>

// Define the pins for SoftwareSerial
const int rxPin = 10;
const int txPin = 11;

SoftwareSerial espSerial(rxPin, txPin); // RX, TX

void setup() {
  // Initialize serial communication with the computer and the ESP32
  Serial.begin(115200);
  espSerial.begin(115200);

  Serial.println("Arduino Uno ready");
  delay(1000); // Time to stabilize

  // Initialize the random number generator
  randomSeed(analogRead(0));
}

void loop() {
  // Generate a random number between 0 and 100
  int randomNumber = random(0, 101);
  String dataToSend = String(randomNumber) + " at " + String(year()) + "/" + String(month()) + "/" + String(day()) + " " + String(hour()) + ":" + String(minute()) + ":" + String(second());

  // Send the number to the ESP32
  espSerial.println(dataToSend);

  // Wait for the confirmation of reception from the ESP32
  delay(500); // Slight pause to wait for the ESP32's response
  if (espSerial.available()) {
    String confirmationMessage = espSerial.readStringUntil('\n');
    if (confirmationMessage.startsWith("Received")) {
      Serial.println("Successfully sent: " + dataToSend);
    } else {
      Serial.println("Sending failed");
    }
  } else {
    Serial.println("No response from ESP32");
  }

  delay(1000); // Wait one second before sending the next number
}
