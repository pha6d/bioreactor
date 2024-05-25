#include <SoftwareSerial.h>

/*
 * Arduino Mega to ESP-12F Communication
 * 
 * This code allows the Arduino Mega to communicate with the ESP-12F module
 * and send data to the server.
 * 
 * Connections:
 * 1. VCC from ESP to 3.3V of Arduino Mega
 * 2. GND from ESP to GND of Arduino Mega
 * 3. TX from ESP to RX of Arduino Mega (use a voltage divider or level shifter)
 * 4. RX from ESP to TX of Arduino Mega
 * 5. EN (CH_PD) to 3.3V
 * 
 * Note: Ensure the ESP-12F is flashed with the proper firmware to connect to WiFi and handle HTTP requests.
 */

// ArduinoMega.ino
// ArduinoMega.ino

void setup() {
    Serial.begin(115200); // Communication with the serial monitor
    Serial1.begin(115200); // Communication with the ESP-12F

    Serial.println("Arduino Mega Ready");
}

void loop() {
    // Generate a random sensor value between 1 and 100
    int sensorValue = random(1, 101);

    // Create a payload to send to the ESP-12F
    String payload = "sensor_value=" + String(sensorValue);

    // Send payload to ESP-12F
    Serial1.println(payload);

    // Debug: Print the payload being sent
    Serial.println("Sending to ESP-12F: " + payload);

    // Wait for a response from the ESP-12F
    unsigned long start = millis();
    while (!Serial1.available() && millis() - start < 2000) {
        // Wait for 2 seconds or until data is available
    }

    if (Serial1.available()) {
        String response = Serial1.readStringUntil('\n');
        Serial.println("ESP-12F: " + response);
    } else {
        Serial.println("No response from ESP-12F");
    }

    delay(1000); // Send data every second
}
