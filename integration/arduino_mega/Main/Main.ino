/*
 * Bioreactor Control System
 * 
 * This program controls a bioreactor system, managing various sensors and actuators.
 * It communicates with an ESP32 module via serial communication to receive commands
 * and send sensor data. The system can perform different operations like mixing,
 * draining, and fermentation based on received commands.
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

// Include interfaces
#include "ActuatorInterface.h"
#include "SensorInterface.h"
#include "ActuatorController.h"

// Include core components
#include "StateMachine.h"
#include "BioReactorManager.h"
#include "SafetySystem.h"
#include "Logger.h"
#include "PIDManager.h"

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
#include "TestActuatorsProgram.h"
#include "DrainProgram.h"
#include "MixProgram.h"
#include "FermentationProgram.h"

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

// Instantiate programs, state machine, and PID controller
DrainProgram drainProgram;
MixProgram mixProgram;
PIDManager pidManager(stirringMotor);
FermentationProgram fermentationProgram(pidManager);
Logger logger;
SafetySystem safetySystem(7, 0.95, 0.1);

StateMachine stateMachine(logger, pidManager);

// Declare global variables
bool stopFlag = false;
String currentProgram = "None";
String programStatus = "Idle";

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

    logger.logInfo("Setup completed");
}

void loop() {
    // Check for incoming commands from ESP32
    if (SerialESP.available() > 0) {
        String receivedData = SerialESP.readStringUntil('\n');
        receivedData.trim();

        logger.logInfo("Received from ESP32: " + receivedData);

        // Check if it's a valid JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, receivedData);

        if (error) {
            // If it's not a valid JSON, treat as a direct command
            executeCommand(receivedData);
        } else {
            // It's a valid JSON, extract information and create the command
            String program = doc["program"];
            
            if (program == "mix") {
                int speed = doc["speed"];
                String command = "mix " + String(speed);
                executeCommand(command);
            } else if (program == "drain") {
                int rate = doc["rate"];
                int duration = doc["duration"];
                String command = "drain " + String(rate) + " " + String(duration);
                executeCommand(command);
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
                executeCommand(command);
            } else if (program == "stop") {
                executeCommand("stop");
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

        executeCommand(command);
    }
    
    // Mise à jour du contrôleur d'actionneurs
    ActuatorController::update();

    // Update state machine
    stateMachine.update(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);

    // Log data every interval
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        logger.logData(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                       waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                       stateMachine.getCurrentProgram(), stateMachine.getCurrentStatus());
    }
    // Short pause to avoid excessive CPU usage
    delay(10);
}

void executeCommand(String command) {
    logger.logInfo("Executing command: " + command);

    if (command.equalsIgnoreCase("help")) {
        printHelp();
    } else if (command.startsWith("test ")) {
        String testCommand = command.substring(5);  // Remove "test " from the beginning
        ActuatorController::executeActuator(testCommand);
    } else if (command.equalsIgnoreCase("tests")) {
        logger.logInfo("Starting tests...");
        stateMachine.startTests(airPump, drainPump, stirringMotor, nutrientPump, basePump, heatingPlate, ledGrowLight,
                                waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
    } else if (command.startsWith("drain")) {
        logger.logInfo("Starting drain...");
        int spaceIndex1 = command.indexOf(' ');
        int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
        int rate = command.substring(spaceIndex1 + 1, spaceIndex2).toInt();
        int duration = command.substring(spaceIndex2 + 1).toInt();
        stateMachine.startDrain(drainPump, rate, duration);
    } else if (command.equalsIgnoreCase("stop")) {
        logger.logInfo("Stopping all...");
        stateMachine.stopAll(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
    } else if (command.startsWith("mix")) {
        logger.logInfo("Starting mix...");
        int spaceIndex = command.indexOf(' ');
        int speed = command.substring(spaceIndex + 1).toInt();
        stateMachine.startMix(stirringMotor, speed);
    } else if (command.startsWith("fermentation")) {
        logger.logInfo("Starting fermentation...");
        // Parse fermentation parameters
        float tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc;
        int duration;
        String experimentName, comment;
        
        sscanf(command.c_str(), "fermentation %f %f %f %f %f %d %s %s", 
               &tempSetpoint, &phSetpoint, &doSetpoint, &nutrientConc, &baseConc, &duration, 
               experimentName.c_str(), comment.c_str());
        
        stateMachine.startFermentation(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                                       waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                                       tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, 
                                       experimentName, comment);
    } else if (command == "test_temp_pid") {
        pidManager.startTest("temp");
    } else if (command == "test_ph_pid") {
        pidManager.startTest("ph");
    } else if (command == "test_do_pid") {
        pidManager.startTest("do");
    } else if (command.startsWith("adjust_volume")) {
        // Implementation of manual volume adjustment
        int firstSpace = command.indexOf(' ');
        int secondSpace = command.indexOf(' ', firstSpace + 1);
        if (firstSpace != -1 && secondSpace != -1) {
            String source = command.substring(firstSpace + 1, secondSpace);
            float amount = command.substring(secondSpace + 1).toFloat();
            // Here you should call a method to adjust the volume
            // For example: bioReactorManager.manuallyAdjustVolume(amount, source);
            logger.logInfo("Manual volume adjustment: " + source + " " + String(amount));
        }
    } else {
        logger.logWarning("Unknown command. Type 'help' for a list of available commands.");
    }
}

void printHelp() {
    Serial.println("Available commands:");
    Serial.println("help - Display this help message");
    Serial.println("test <actuator> <value> <duration> - Test a specific actuator");
    Serial.println("  Available actuators:");
    Serial.println("    basePump <flow_rate_ml_per_min> <duration_seconds>");
    Serial.println("    nutrientPump <flow_rate_ml_per_min> <duration_seconds>");
    Serial.println("    airPump <speed_0_255> <duration_seconds>");
    Serial.println("    drainPump <speed_0_255> <duration_seconds>");
    Serial.println("    stirringMotor <speed_0_255> <duration_seconds>");
    Serial.println("    heatingPlate <temperature_celsius> <duration_seconds>");
    Serial.println("    ledGrowLight <intensity_0_255> <duration_seconds>");
    Serial.println("tests - Run all predefined tests");
    Serial.println("drain <rate> <duration> - Start draining");
    Serial.println("stop - Stop all actuators");
    Serial.println("mix <speed> - Start mixing");
    Serial.println("fermentation <temp> <ph> <do> <nutrient_conc> <base_conc> <duration> <experiment_name> <comment> - Start fermentation");
    Serial.println("test_temp_pid - Test temperature PID controller");
    Serial.println("test_ph_pid - Test pH PID controller");
    Serial.println("test_do_pid - Test dissolved oxygen PID controller");
    Serial.println("adjust_volume <source> <amount> - Manually adjust volume (e.g., adjust_volume NaOH 10.5)");
}