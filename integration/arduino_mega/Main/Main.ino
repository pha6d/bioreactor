/*
 * main.cpp
 * This file demonstrates how to use various sensors and actuators in the bioréacteur system.
 * It includes sensors for water and air temperature, dissolved oxygen, pH, turbidity, air flow, and controls for various pumps and motors.
 * The sensor readings and actuator states are printed to the Serial monitor every second.
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

#include "PeristalticPump.h"
#include "DCPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"

#include "PHSensor.h"
#include "OxygenSensor.h"
#include "TurbiditySensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PT100Sensor.h"
#include "AirFlowSensor.h

// Define the pins for SoftwareSerial
SoftwareSerial espSerial(11, 12); // RX, TX

// Create pump objects with specific pin assignments and minimum PWM values
DCPump airPump(5, 6, 10, "Air Pump"); // Air pump: PWM pin 5, Relay pin 6
DCPump drainPump(3, 4, 15, "Drain Pump"); // Drain pump: PWM pin 3, Relay pin 4

// Create peristaltic pump objects
PeristalticPump nutrientPump(0x61, 7, 105.0, "Nutrient Pump"); // Nutrient pump: DAC address 0x61, Relay pin 7, max flow rate 105 ml/min
PeristalticPump basePump(0x60, 8, 105.0, "Base Pump"); // Base pump: DAC address 0x60, Relay pin 8, max flow rate 105 ml/min

// Create stirring motor object
StirringMotor stirringMotor(9, 10); // Stirring motor: PWM pin 9, Relay pin 10

// Create an instance of the HeatingPlate class
HeatingPlate heatingPlate(13, "Heating Plate"); // Heating plate (12 volt 13 Watts): Relay pin 13  

// Create sensor objects
PHSensor phSensor(A1);
TurbiditySensor turbiditySensor(A2);
OxygenSensor oxygenSensor(A3, &waterTempSensor); // Dissolved Oxygen sensor: Analog pin A3, uses water temperature sensor for compensation
PT100Sensor waterTempSensor(22, 23, 24, 25); // Water temperature sensor: CS pin 22, DI pin 23, DO pin 24, CLK pin 25
DS18B20TemperatureSensor airTempSensor(52); // Air temperature sensor: Digital pin 52
AirFlowSensor airFlowSensor(26); // Air flow meter: Digital pin 26

void setup() {
    Serial.begin(115200); // Initialize serial communication at 115200 baud rate
    espSerial.begin(115200);

    Serial.println("Setup started");

    // Initialize sensors and actuators
  
    phSensor.begin();
    turbiditySensor.begin();
    oxygenSensor.begin();
    airTempSensor.begin();
    airTempSensor.begin();

    nutrientPump.begin();
    basePump.begin();
    drainPump.begin();
    airPump.begin();
    stirringMotor.begin();
    heatingPlate.control(false); // Initialize the heating plate to be off
    ledGrowLight.control(false); // Initialize the LED grow light to be off

    Serial.println("Setup completed");
}

void loop() {

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

    // Example of controlling the heating plate (turn on)
    heatingPlate.control(true);
    delay(2000);
    heatingPlate.control(false);
    delay(2000);

    // Example of controlling the LED grow light (turn on)
    ledGrowLight.control(true);
    delay(2000);
    ledGrowLight.control(false);
    delay(2000);
*/


    // Read and print water temperature sensor value
    float waterTemperature = waterTempSensor.readValue();
    Serial.print("Water Temperature: ");
    Serial.print(waterTemperature);
    Serial.println(" °C");

    // Read and print air temperature sensor value
    float airTemperature = airTempSensor.readValue();
    Serial.print("Air Temperature: ");
    Serial.print(airTemperature);
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

    // Read and print air flow sensor value
    float flowRate = airFlowSensor.readValue();
    if (flowRate != -1.0) { // Check if the flow rate was updated
        Serial.print("Air Flow Rate: ");
        Serial.print(flowRate);
        Serial.println(" L/min");
    }


    // Sends collected data to the ESP32 via serial communication
    String data = String("Water_Temperature:") + String(waterTemperature) +
                  ",Air_Temperature:" + String(airTemperature) +
                  ",pH:" + String(pHValue) +
                  ",Turbidity_Voltage:" + String(turbidityValue) +
                  ",Dissolved_Oxygen:" + String(DOValue) +
                  ",Air_Flow_Rate:" + String(flowRate);

    espSerial.println(data);
    Serial.println("Data sent to ESP32: " + data);

    delay(1000); // Wait for 1 second before the next loop iteration
}