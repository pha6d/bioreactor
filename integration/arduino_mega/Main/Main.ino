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

// main.ino
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "SensorController.h"
#include "ActuatorController.h"
#include "StateMachine.h"
#include "VolumeManager.h"
#include "SafetySystem.h"
#include "Logger.h"
#include "PIDManager.h"
#include "CommandHandler.h"

#include "TestsProgram.h"
#include "DrainProgram.h"
#include "MixProgram.h"
#include "FermentationProgram.h"

// Define serial port for communication with ESP32
#define SerialESP Serial1

// Sensor declarations
PT100Sensor waterTempSensor(22, 23, 24, 25, "waterTempSensor");
DS18B20TemperatureSensor airTempSensor(52, "airTempSensor");
PHSensor phSensor(A1, "phSensor");
TurbiditySensor turbiditySensor(A2, "turbiditySensor");
OxygenSensor oxygenSensor(A3, &waterTempSensor, "oxygenSensor");
AirFlowSensor airFlowSensor(26, "airFlowSensor");

// Actuator declarations
DCPump airPump(5, 6, 10, "airPump");
DCPump drainPump(3, 4, 15, "drainPump");
PeristalticPump nutrientPump(0x61, 7, 1, 105.0, "nutrientPump");
PeristalticPump basePump(0x60, 8, 1, 105.0, "basePump");
StirringMotor stirringMotor(9, 10, 390, 1000,"stirringMotor"); // (390, 1500); 1000max sans déraillage
HeatingPlate heatingPlate(12, false, "heatingPlate");
LEDGrowLight ledGrowLight(27, "ledGrowLight");

// System components
Logger logger;
PIDManager pidManager;
VolumeManager volumeManager(1.0, 0.95, 0.1);
SafetySystem safetySystem(1.0, 0.95, 0.1);
StateMachine stateMachine(logger, pidManager, volumeManager);

// Program declarations
TestsProgram testsProgram(pidManager);
DrainProgram drainProgram;
MixProgram mixProgram;
FermentationProgram fermentationProgram(pidManager, volumeManager);

CommandHandler commandHandler(stateMachine, safetySystem, volumeManager, logger, pidManager);

unsigned long previousMillis = 0;
const long interval = 30000; // Interval for logging (30 seconds)

void setup() {
    Serial.begin(115200);  // Initialize serial communication for debugging
    SerialESP.begin(9600); // Initialize serial communication with ESP32

    Logger::log(LogLevel::INFO, "Setup started");

    // Initialize sensors
    SensorController::initialize(waterTempSensor, airTempSensor, phSensor,
                                 turbiditySensor, oxygenSensor, airFlowSensor);
    SensorController::beginAll();

    // Initialize actuators
    ActuatorController::initialize(airPump, drainPump, nutrientPump, basePump,
                                   stirringMotor, heatingPlate, ledGrowLight);
    ActuatorController::beginAll();
    
    safetySystem.setLogger(&logger); //This allows the SafetySystem to use the same logger

    // Add programs to the state machine
    stateMachine.addProgram("Tests", &testsProgram);
    stateMachine.addProgram("Drain", &drainProgram);
    stateMachine.addProgram("Mix", &mixProgram);
    stateMachine.addProgram("Fermentation", &fermentationProgram);

    // Initialisation of the PIDManager to define hysteresis values
    pidManager.initialize(2.0, 5.0, 1.0, 2.0, 5.0, 1.0, 2.0, 5.0, 1.0);
    pidManager.setHysteresis(0.5, 0.05, 1.0);
    Logger::log(LogLevel::INFO, "PID setup");

    Logger::log(LogLevel::INFO, "Setup completed");
}

void loop() {
    // Check for incoming commands from ESP32
    if (SerialESP.available() > 0) {
        String receivedData = SerialESP.readStringUntil('\n');
        receivedData.trim();

        Logger::log(LogLevel::INFO, "Received from ESP32: " + receivedData);

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
                Logger::log(LogLevel::WARNING, "Unknown program: " + program);
            }
        }
    }

    // Check for incoming commands from Arduino Serial Monitor
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        Logger::log(LogLevel::INFO, "Received from Serial Monitor: " + command);

        commandHandler.executeCommand(command);
    }
    
    // Update state machine
    stateMachine.update();

    // Update PID manager
    pidManager.updateAllPIDControllers();

    // Check safety limits
    //safetySystem.checkLimits();

    // Log data every interval
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        logger.logData(stateMachine.getCurrentProgram(), String(static_cast<int>(stateMachine.getCurrentState())));
    }

    // Short pause to avoid excessive CPU usage
    delay(10);
}