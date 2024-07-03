/*
  Instructions to connect the relay and control a 5V LED grow light (chip 2835) with ESP8266:

  1. Connections for the relay to ESP8266:
     - Relay VCC: Connect to 3.3V on the ESP8266.
     - Relay GND: Connect to GND on the ESP8266.
     - Relay IN (control signal): Connect to digital pin D1 (GPIO5) on the ESP8266.

  2. Connections for the LED light to the relay:
     - Red Wire (5V) from LED light: Connect to the normally open (NO) terminal of the relay.
     - Black Wire (GND) from LED light: Connect to the GND of the power source.
     - Power source 5V: Connect the positive terminal to the common (COM) terminal of the relay.
     - Power source GND: Connect the negative terminal to the GND of the ESP8266 and the GND of the LED light.

  This code will turn the LED light on or off based on commands received from the serial monitor.

  Installation instructions for ESP8266 with CH340 chip:

  1. Download and install the CH340 driver: 
     https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all

  2. Add the following URL to the Arduino IDE for additional boards manager URLs:
     http://arduino.esp8266.com/stable/package_esp8266com_index.json

  3. Install the ESP8266 board in the Arduino IDE boards manager:
     Go to "Tools" > "Board" > "Boards Manager", search for "esp8266", and install "esp8266 by ESP8266 Community".

  The ESP-12F Module Capabilities:

  1. Wi-Fi: The ESP-12F has built-in Wi-Fi capabilities, allowing it to connect to Wi-Fi networks and act as an access point.
  2. GPIO: It has multiple General Purpose Input/Output (GPIO) pins for connecting sensors, LEDs, buttons, and other peripherals.
  3. PWM: Pulse Width Modulation for dimming LEDs or controlling servos.
  4. ADC: Analog to Digital Converter for reading analog sensors.
  5. I2C: Inter-Integrated Circuit for communication with other I2C devices.
  6. SPI: Serial Peripheral Interface for communication with SPI devices.
  7. UART: Universal Asynchronous Receiver/Transmitter for serial communication.
  8. Deep Sleep: Power-saving mode to reduce power consumption during inactivity.
*/

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

// Constants for relay and built-in LED pins
const int relayPin = 5; // GPIO5 (D1 on ESP8266)
const int builtInLed = 2; // Built-in LED (GPIO2)

// Define NTP client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600 * 2, 1800000); // Update every half hour, GMT+2 for CEST

// Define the on and off periods in hours (16 hours on, 8 hours off)
const int onStartHour = 5; // 5 AM
const int offStartHour = 21; // 21 PM

// Variable to store manual control state
bool manualControl = false;
bool manualState = false;

void setup() {
  Serial.begin(115200);  // Start the serial communication
  pinMode(relayPin, OUTPUT); // Set the relay pin as an output
  pinMode(builtInLed, OUTPUT); // Set the built-in LED pin as an output

  // Initialize the relay and built-in LED to off
  digitalWrite(relayPin, HIGH); // Relay OFF (HIGH turns OFF the relay for ESP8266)
  digitalWrite(builtInLed, HIGH); // LED OFF (HIGH turns OFF the LED for ESP8266)

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP client
  timeClient.begin();

  // Update the time and print it
  timeClient.update();
  Serial.print("Current time: ");
  Serial.println(timeClient.getFormattedTime());
}

void loop() {
  // Update time
  timeClient.update();
  
  // Print the current time
  Serial.print("Current time: ");
  Serial.println(timeClient.getFormattedTime());

  // Check for serial commands to turn the grow light on or off manually
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any leading or trailing whitespace

    if (command == "ON") {
      manualControl = true;
      manualState = true;
      digitalWrite(relayPin, LOW); // Relay ON (LOW turns ON the relay for ESP8266)
      digitalWrite(builtInLed, LOW); // LED ON (LOW turns ON the LED for ESP8266)
      Serial.println("Manual: LED Grow Light is turned ON.");
    } else if (command == "OFF") {
      manualControl = true;
      manualState = false;
      digitalWrite(relayPin, HIGH); // Relay OFF (HIGH turns OFF the relay for ESP8266)
      digitalWrite(builtInLed, HIGH); // LED OFF (HIGH turns OFF the LED for ESP8266)
      Serial.println("Manual: LED Grow Light is turned OFF.");
    } else if (command == "AUTO") {
      manualControl = false;
      Serial.println("Switched to automatic control.");
    } else {
      Serial.println("Unknown command. Please send 'ON', 'OFF', or 'AUTO'.");
    }
  }

  // Automatic control based on time if manual control is not active
  if (!manualControl) {
    // Get the current hour of the day
    int currentHour = (timeClient.getEpochTime() % 86400L) / 3600;

    if (currentHour >= onStartHour && currentHour < offStartHour) {
      // Turn the lights on
      digitalWrite(relayPin, LOW); // Relay ON (LOW turns ON the relay for ESP8266)
      digitalWrite(builtInLed, LOW); // LED ON (LOW turns ON the LED for ESP8266)
      Serial.println("Automatic: LED Grow Light is ON");
    } else {
      // Turn the lights off
      digitalWrite(relayPin, HIGH); // Relay OFF (HIGH turns OFF the relay for ESP8266)
      digitalWrite(builtInLed, HIGH); // LED OFF (HIGH turns OFF the LED for ESP8266)
      Serial.println("Automatic: LED Grow Light is OFF");
    }
  }

  // Wait for an hour before checking again
  delay(1800000); // 30 minutes
}
