/*
 * main.cpp
 * This file serves as the main entry point for the Arduino project.
 * It initializes and controls multiple actuators and sensors, allowing for flexible operation and data collection. Data are send to ESP32 via serial communication.
 *
 * Physical modules used:
 * - Arduino Mega 2560
 * - Relay modules
 * - PW1VA01 PWM to Voltage converters
 * - Motor speed controllers
 * - DC pumps (air pump and liquid drain pump)
 * - PWM fan for stirring (e.g., be quiet! Pure Wings 2 PWM 120 mm 1500rpm - 4 wires)
 * - MCP4725 DAC modules
 * - Peristaltic pump (Pulse Generation Control Board + two-phase DC stepping driver + stepper motor)
 * - DS18B20 Temperature Sensor
 * - Gravity: Analog pH Meter V2
 * - Gravity: Analog Turbidity Sensor
 * - Gravity: Analog Dissolved Oxygen Sensor / Meter Kit for Arduino
 */

#include <SoftwareSerial.h>
#include <Arduino.h>
#include "DCPump.h"
#include "StirringMotor.h"
#include "PeristalticPump.h"
#include "TemperatureSensor.h"
#include "PHSensor.h"
#include "TurbiditySensor.h"
#include "OxygenSensor.h"

// Define the pins for SoftwareSerial
const int rxPin = 11;
const int txPin = 12;

SoftwareSerial espSerial(rxPin, txPin); // RX, TX

// Create pump objects with specific pin assignments and minimum PWM values
DCPump airPump(5, 6, 10, "Air Pump"); // Air pump: PWM pin 5, Relay pin 6
DCPump drainPump(3, 4, 15, "Drain Pump"); // Drain pump: PWM pin 3, Relay pin 4

// Create peristaltic pump objects
PeristalticPump nutrientPump(0x61, 7, 105.0, "Nutrient Pump"); // Nutrient pump: DAC address 0x61, Relay pin 7, max flow rate 105 ml/min
PeristalticPump basePump(0x60, 8, 105.0, "Base Pump"); // Base pump: DAC address 0x60, Relay pin 8, max flow rate 105 ml/min

// Create stirring motor object
StirringMotor stirringMotor(9, 10); // Stirring motor: PWM pin 9, Relay pin 10

// Create sensor objects
TemperatureSensor tempSensor(A4);
PHSensor phSensor(A1);
TurbiditySensor turbiditySensor(A2);
OxygenSensor oxygenSensor(A3, A4);

void setup() {
    Serial.begin(115200); // Initialize serial communication at 115200 baud rate
    espSerial.begin(115200);

    Serial.println("Setup started");

    // Initialize sensors and actuators
    tempSensor.begin();
    phSensor.begin();
    turbiditySensor.begin();
    oxygenSensor.begin();
    nutrientPump.begin();
    basePump.begin();

    Serial.println("Setup completed");
}

void loop() {
    Serial.println("Loop started");
/*
    // Example usage of the air pump
    airPump.control(true, 50);  // Turn the air pump on at 50% speed
    delay(5000);                // Run for 5 seconds
    airPump.control(false, 0);  // Turn the air pump off
    delay(2000);                // Off for 2 seconds

    // Example usage of the drain pump
    drainPump.control(true, 80);  // Turn the drain pump on at 80% speed
    delay(5000);                  // Run for 5 seconds
    drainPump.control(false, 0);  // Turn the drain pump off
    delay(2000);                  // Off for 2 seconds

    // Example usage of the stirring motor
    stirringMotor.control(true, 1500);  // Turn the stirring motor on at 1500 RPM
    delay(5000);                        // Run for 5 seconds
    stirringMotor.control(false, 0);    // Turn the stirring motor off
    delay(2000);                        // Off for 2 seconds

    // Example usage of the nutrient pump
    nutrientPump.control(true, 50);  // Turn the nutrient pump on at 50 ml/min
    delay(5000);                     // Run for 5 seconds
    nutrientPump.control(false, 0);  // Turn the nutrient pump off
    delay(2000);                     // Off for 2 seconds

    // Example usage of the base pump
    basePump.control(true, 30);  // Turn the base pump on at 30 ml/min
    delay(5000);                 // Run for 5 seconds
    basePump.control(false, 0);  // Turn the base pump off
    delay(2000);                 // Off for 2 seconds
*/

    // Read and print temperature sensor value
    float temperature = tempSensor.readValue();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Read and print pH sensor value
    float pHValue = phSensor.readValue();
    Serial.print("pH Value: ");
    Serial.println(pHValue);

    // Read and print turbidity sensor value
    float turbidityValue = turbiditySensor.readValue();
    Serial.print("Turbidity Voltage: ");
    Serial.println(turbidityValue);

    // Read and print dissolved oxygen sensor value
    float DOValue = oxygenSensor.readValue();
    Serial.print("Dissolved Oxygen: ");
    Serial.print(DOValue);
    Serial.println(" mg/L");

    Serial.println("Loop completed");

    // Sends collected data to the ESP32 via serial communication
    String data = String("Temperature:") + String(temperature) +
                  ",pH:" + String(pHValue) +
                  ",Turbidity_Voltage:" + String(turbidityValue) +
                  ",O2:" + String(DOValue);
    
    espSerial.println(data);
    Serial.println("Data sent to ESP32: " + data);

    delay(1000); // Wait for 1 second before the next loop iteration
}
