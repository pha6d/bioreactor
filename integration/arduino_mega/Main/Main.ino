/*
 * Bioreactor Control System
 * 
 * This program controls a bioreactor system, managing various sensors and actuators.
 * It communicates with an ESP32 module via serial communication to receive commands
 * and send sensor data. The system can perform different operations like mixing,
 * draining, fermentation, and individual PID control based on received commands.
 * 
 * Key components:
 * - Sensors: pH, oxygen, turbidity, temperature (water and air), air flow
 * - Actuators: pumps (air, drain, nutrient, base), stirring motor, heating plate, LED grow light
 * - Communication: Serial interface with ESP32
 * - State Machine: Manages the overall state and operations of the bioreactor
 * 
 * The program continuously checks for incoming commands, updates the state machine,
 * and logs sensor data at regular intervals.
 */

// Include libraries
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Include core components
#include "StateMachine.h"
#include "VolumeManager.h"
#include "SafetySystem.h"
#include "Logger.h"
#include "PIDManager.h"
#include "CommandHandler.h"

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
#include "DrainProgram.h"
#include "MixProgram.h"
#include "TestActuatorsProgram.h"
#include "FermentationProgram.h"
#include "PIDTestProgram.h"

// Define serial port for communication with ESP32
#define SerialESP Serial1

// Create objects with specific pin assignments and values
DCPump airPump(5, 6, 10, "Air Pump");
DCPump drainPump(3, 4, 15, "Drain Pump");
PeristalticPump nutrientPump(0x61, 7, 105.0, "Nutrient Pump");
PeristalticPump basePump(0x60, 8, 105.0, "Base Pump");
StirringMotor stirringMotor(9, 10, "Stirring Motor", 390, 1500);
HeatingPlate heatingPlate(13, "Heating Plate");
LEDGrowLight ledGrowLight(27, "LED Grow Light");

PT100Sensor waterTempSensor(22, 23, 24, 25);
DS18B20TemperatureSensor airTempSensor(52);
PHSensor phSensor(A1);
TurbiditySensor turbiditySensor(A2);
OxygenSensor oxygenSensor(A3, &waterTempSensor);
AirFlowSensor airFlowSensor(26);

// Instantiate programs, state machine, PID controller, etc.
Logger logger;
PIDManager pidManager(&stirringMotor, &heatingPlate);
VolumeManager volumeManager(1.0, 0.95, 0.1, nutrientPump, basePump, drainPump);
SafetySystem safetySystem(1.0, 0.95, 0.1); // totalVolume, maxVolumePercent, minVolume

StateMachine stateMachine(logger, pidManager, volumeManager);

DrainProgram drainProgram;
MixProgram mixProgram;
TestActuatorsProgram testActuatorsProgram;
FermentationProgram fermentationProgram(pidManager, volumeManager);
PIDTestProgram pidTestProgram(pidManager);

CommandHandler commandHandler(stateMachine, safetySystem, volumeManager, logger,
                              airPump, drainPump, nutrientPump, basePump,
                              stirringMotor, heatingPlate, ledGrowLight,
                              waterTempSensor, airTempSensor, phSensor,
                              turbiditySensor, oxygenSensor, airFlowSensor,
                              pidManager);

unsigned long previousMillis = 0;
const long interval = 30000; // Interval for logging (30 seconds)

void setup() {
    Serial.begin(115200);  // Initialize serial communication for debugging
    SerialESP.begin(9600); // Initialize serial communication with ESP32

    logger.logInfo("Setup started");

    // Initialize sensors and actuators
    phSensor.begin();
    turbiditySensor.begin();
    oxygenSensor.begin();
    airTempSensor.begin();
    waterTempSensor.begin();
    airFlowSensor.begin();
    nutrientPump.begin();
    basePump.begin();
    heatingPlate.control(false);
    ledGrowLight.control(false);

    pidManager.initialize(2.0, 5.0, 1.0, 2.0, 5.0, 1.0, 2.0, 5.0, 1.0);
    safetySystem.setLogger(&logger);

    // Add programs to the state machine
    stateMachine.addProgram(&drainProgram);
    stateMachine.addProgram(&mixProgram);
    stateMachine.addProgram(&testActuatorsProgram);
    stateMachine.addProgram(&fermentationProgram);
    stateMachine.addProgram(&pidTestProgram);

    // Set up PIDTestProgram
    pidTestProgram.setWaterTempSensor(&waterTempSensor);
    pidTestProgram.setPhSensor(&phSensor);
    pidTestProgram.setOxygenSensor(&oxygenSensor);
    pidTestProgram.setHeatingPlate(&heatingPlate);
    pidTestProgram.setAirPump(&airPump);

    logger.logInfo("Setup completed");
}

void loop() {
    // Check for incoming commands from ESP32
    if (SerialESP.available() > 0) {
        String receivedData = SerialESP.readStringUntil('\n');
        receivedData.trim();

        logger.logInfo("Received from ESP32: " + receivedData);

        // Check if it's a valid JSON
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, receivedData);

        if (error) {
            // If it's not a valid JSON, treat as a direct command
            commandHandler.executeCommand(receivedData);
        } else {
            // It's a valid JSON, extract information and create the command
            String program = doc["program"];
            
            if (program == "mix") {
                int speed = doc["speed"];
                String command = "mix " + String(speed);
                commandHandler.executeCommand(command);
            } else if (program == "drain") {
                int rate = doc["rate"];
                int duration = doc["duration"];
                String command = "drain " + String(rate) + " " + String(duration);
                commandHandler.executeCommand(command);
            } else if (program == "fermentation") {
                float tempSetpoint = doc["temperature"];
                float phSetpoint = doc["pH"];
                float doSetpoint = doc["dissolvedOxygen"];
                float nutrientConc = doc["nutrientConcentration"];
                float baseConc = doc["baseConcentration"];
                int duration = doc["duration"];
                String experimentName = doc["experimentName"];
                String comment = doc["comment"];
                String command = "fermentation " + String(tempSetpoint) + " " + String(phSetpoint) + " " +
                                 String(doSetpoint) + " " + String(nutrientConc) + " " + String(baseConc) + " " +
                                 String(duration) + " " + experimentName + " " + comment;
                commandHandler.executeCommand(command);
            } else if (program == "stop") {
                commandHandler.executeCommand("stop");
            } else {
                logger.logWarning("Unknown program: " + program);
            }
        }
    }

    // Check for incoming commands from Arduino Serial Monitor
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        logger.logInfo("Received from Serial Monitor: " + command);

        commandHandler.executeCommand(command);
    }
    
    // Update state machine
    stateMachine.update();

    // Update PID manager
    pidManager.updateAll(waterTempSensor.readValue(), phSensor.readValue(), oxygenSensor.readValue());

    // Check safety limits
    safetySystem.checkLimits(waterTempSensor.readValue(), 
                             airTempSensor.readValue(), 
                             phSensor.readValue(), 
                             oxygenSensor.readValue(), 
                             turbiditySensor.readValue(), 
                             volumeManager.getCurrentVolume());

    // Log data every interval
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        logger.logData(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
               waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
               stateMachine.getCurrentProgram(), String(static_cast<int>(stateMachine.getCurrentState())));
    }

    // Short pause to avoid excessive CPU usage
    delay(10);
}