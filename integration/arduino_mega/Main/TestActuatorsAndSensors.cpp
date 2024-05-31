/*
 * TestActuatorsAndSensors.cpp
 * This file provides the implementation of the function to test all actuators and sensors in the bioreactor system.
 */

#include "TestActuatorsAndSensors.h"
#include <SoftwareSerial.h> 

void runTestActuatorsAndSensors(DCPump &airPump, DCPump &drainPump, StirringMotor &stirringMotor, PeristalticPump &nutrientPump, PeristalticPump &basePump, HeatingPlate &heatingPlate, LEDGrowLight &ledGrowLight, PT100Sensor &waterTempSensor, DS18B20TemperatureSensor &airTempSensor, PHSensor &phSensor, TurbiditySensor &turbiditySensor, OxygenSensor &oxygenSensor, AirFlowSensor &airFlowSensor) {
    
    Serial.println(), Serial.println("run TESTS program");

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
    delay(7000);
    heatingPlate.control(false);
    delay(2000);

    // Example of controlling the LED grow light (turn on)
    ledGrowLight.control(true);
    delay(5000);
    ledGrowLight.control(false);
    delay(2000);

    // Read and print sensor values
    float waterTemperature = waterTempSensor.readValue();
    Serial.print("Water Temperature: ");
    Serial.print(waterTemperature);
    Serial.println(" °C");

    float airTemperature = airTempSensor.readValue();
    Serial.print("Air Temperature: ");
    Serial.print(airTemperature);
    Serial.println(" °C");

    float pHValue = phSensor.readValue();
    Serial.print("pH Value: ");
    Serial.println(pHValue);

    float turbidityValue = turbiditySensor.readValue();
    Serial.print("Turbidity Voltage: ");
    Serial.println(turbidityValue);

    float DOValue = oxygenSensor.readValue();
    Serial.print("Dissolved Oxygen: ");
    Serial.print(DOValue);
    Serial.println(" mg/L");

    float flowRate = airFlowSensor.readValue();
    if (flowRate != -1.0) { // Check if the flow rate was updated
        Serial.print("Air Flow Rate: ");
        Serial.print(flowRate);
        Serial.println(" L/min");
    }
}
