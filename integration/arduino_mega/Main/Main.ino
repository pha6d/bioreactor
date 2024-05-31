#include <SoftwareSerial.h>
#include <Arduino.h>

// Include interfaces
#include "ActuatorInterface.h"
#include "SensorInterface.h"

// Include actuators
#include "PeristalticPump.h"
#include "DCPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"

// Include sensors
#include "PHSensor.h"
#include "OxygenSensor.h"
#include "TurbiditySensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PT100Sensor.h"
#include "AirFlowSensor.h"

// Include programs
#include "TestActuatorsAndSensors.h" // Include the header file for the test program
#include "Drain.h"  // Include the header file for the drain program
#include "Stop.h"   // Include the header file for the stop program
#include "Mix.h"    // Include the header file for the mix program
#include "Fermentation.h" // Include the header file for the fermentation program

// Define the pins for SoftwareSerial
SoftwareSerial espSerial(11, 12); // RX, TX

// Declare stopFlag as a global variable
bool stopFlag = false;

// Create objects with specific pin assignments and values if applicable 
    // Actuators
DCPump airPump(5, 6, 10, "Air Pump"); // Air pump: PWM pin 5, Relay pin 6
DCPump drainPump(3, 4, 15, "Drain Pump"); // Drain pump: PWM pin 3, Relay pin 4
PeristalticPump nutrientPump(0x61, 7, 105.0, "Nutrient Pump"); // Nutrient pump: DAC address 0x61, Relay pin 7, max flow rate 105 ml/min
PeristalticPump basePump(0x60, 8, 105.0, "Base Pump"); // Base pump: DAC address 0x60, Relay pin 8, max flow rate 105 ml/min
StirringMotor stirringMotor(9, 10); // Stirring motor: PWM pin 9, Relay pin 10
HeatingPlate heatingPlate(13, "Heating Plate"); // Heating plate (12 volt 13 Watts): Relay pin 13  
LEDGrowLight ledGrowLight(27, "LED Grow Light"); // LED Grow Light: Relay pin 27
    // Sensors
PT100Sensor waterTempSensor(22, 23, 24, 25); // Water temperature sensor: CS pin 22, DI pin 23, DO pin 24, CLK pin 25
DS18B20TemperatureSensor airTempSensor(52); // Air temperature sensor: Digital pin 52
PHSensor phSensor(A1);
TurbiditySensor turbiditySensor(A2);
OxygenSensor oxygenSensor(A3, &waterTempSensor); // Dissolved Oxygen sensor: Analog pin A3, uses water temperature sensor for compensation
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
    waterTempSensor.begin();
    airFlowSensor.begin();
    nutrientPump.begin();
    basePump.begin();
    heatingPlate.control(false); // Initialize the heating plate to be off
    ledGrowLight.control(false); // Initialize the LED grow light to be off
    runStop(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);

    Serial.println("Setup completed");
    Serial.println();
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n'); // Read the incoming command
        command.trim(); // Remove any trailing whitespace

        if (command.equalsIgnoreCase("tests")) {
            runTestActuatorsAndSensors(airPump, drainPump, stirringMotor, nutrientPump, basePump, heatingPlate, ledGrowLight, waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
        }
        // Add the drain command
        else if (command.startsWith("drain")) {
            // Extract the rate and duration from the command
            int spaceIndex1 = command.indexOf(' ');
            int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
            int rate = command.substring(spaceIndex1 + 1, spaceIndex2).toInt();
            int duration = command.substring(spaceIndex2 + 1).toInt();
            
            runDrain(drainPump, rate, duration);
        }
        // Add the stop command
        else if (command.equalsIgnoreCase("stop")) {
            runStop(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
        }
        // Add the mix command
        else if (command.startsWith("mix")) {
            // Extract the speed from the command
            int spaceIndex = command.indexOf(' ');
            int speed = command.substring(spaceIndex + 1).toInt();
            
            runMix(stirringMotor, speed);
        }
        // Add the fermentation command
        else if (command.startsWith("fermentation")) {
            // Extract the parameters from the command
            int spaceIndex1 = command.indexOf(' ');
            int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
            int spaceIndex3 = command.indexOf(' ', spaceIndex2 + 1);
            int spaceIndex4 = command.indexOf(' ', spaceIndex3 + 1);
            int spaceIndex5 = command.indexOf(' ', spaceIndex4 + 1);
            int spaceIndex6 = command.indexOf(' ', spaceIndex5 + 1);
            int spaceIndex7 = command.indexOf(' ', spaceIndex6 + 1);
            int spaceIndex8 = command.indexOf(' ', spaceIndex7 + 1);
            int spaceIndex9 = command.indexOf(' ', spaceIndex8 + 1);

            float tempSetpointVal = command.substring(spaceIndex1 + 1, spaceIndex2).toFloat();
            float phSetpointVal = command.substring(spaceIndex2 + 1, spaceIndex3).toFloat();
            float doSetpointVal = command.substring(spaceIndex3 + 1, spaceIndex4).toFloat();
            float nutrientConc = command.substring(spaceIndex4 + 1, spaceIndex5).toFloat();
            float baseConc = command.substring(spaceIndex5 + 1, spaceIndex6).toFloat();
            int duration = command.substring(spaceIndex6 + 1, spaceIndex7).toInt();
            String experimentName = command.substring(spaceIndex7 + 1, spaceIndex8);
            String experimentComment = command.substring(spaceIndex8 + 1);

            runFermentation(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                            waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                            tempSetpointVal, phSetpointVal, doSetpointVal, nutrientConc, baseConc, duration, experimentName, experimentComment);
        }
        // Add other command cases here
        else {
            Serial.println("Unknown command.");
        }
    }
}
