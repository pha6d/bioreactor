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

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "StateMachine.h"
#include "Logger.h"
#include "PIDController.h"

// Include interfaces
#include "ActuatorInterface.h"
#include "SensorInterface.h"
#include "ActuatorController.h"

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
#include "TestActuatorsAndSensors.h"
#include "Drain.h"
#include "Mix.h"
#include "Fermentation.h"

// Define serial port for communication with ESP32
#define SerialESP Serial1

// Create objects with specific pin assignments and values if applicable
// Actuators
DCPump airPump(5, 6, 10, "Air Pump");
DCPump drainPump(3, 4, 15, "Drain Pump");
PeristalticPump nutrientPump(0x61, 7, 105.0, "Nutrient Pump");
PeristalticPump basePump(0x60, 8, 105.0, "Base Pump");
StirringMotor stirringMotor(9, 10, "Stirring Motor");
HeatingPlate heatingPlate(13, "Heating Plate");
LEDGrowLight ledGrowLight(27, "LED Grow Light");

// Sensors
PT100Sensor waterTempSensor(22, 23, 24, 25);
DS18B20TemperatureSensor airTempSensor(52);
PHSensor phSensor(A1);
TurbiditySensor turbiditySensor(A2);
OxygenSensor oxygenSensor(A3, &waterTempSensor);
AirFlowSensor airFlowSensor(26);

// Declare global variables
bool stopFlag = false;
String currentProgram = "None";
String programStatus = "Idle";

// Instantiate programs, state machine, and PID controller
DrainProgram drainProgram;
MixProgram mixProgram;
StateMachine stateMachine;
PIDController pidController;

// Variables for logger timing
unsigned long previousMillis = 0;
const long interval = 30000; // Interval for logging (30 seconds)

void setup() {
    Serial.begin(115200);  // Initialize serial communication for debugging
    SerialESP.begin(9600); // Initialize serial communication with ESP32

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
    heatingPlate.control(false);
    ledGrowLight.control(false);

    pidController.initialize(2.0, 5.0, 1.0, 2.0, 5.0, 1.0, 2.0, 5.0, 1.0);

    Serial.println("Setup completed");
    Serial.println();
}

void loop() {
    // Check for incoming commands from ESP32
    if (SerialESP.available() > 0) {
        String receivedData = SerialESP.readStringUntil('\n');
        receivedData.trim();

        Serial.print("Received from ESP32: ");
        Serial.println(receivedData);

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
                Serial.println("Unknown program: " + program);
            }
        }
    }

    // Check for incoming commands from Arduino Serial Monitor
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        Serial.print("Received from Serial Monitor: ");
        Serial.println(command);

        executeCommand(command);
    }

    // Update PID controllers
    if (pidController.isTestRunning()) {
        pidController.updateTest();
    } else {
        pidController.updateAll(waterTempSensor.readValue(), 
                                phSensor.readValue(), 
                                oxygenSensor.readValue());
    }

    // Log data every interval
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        logData(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                currentProgram, programStatus);
    }

    // Update the state machine
    stateMachine.update(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
    // Update ActuatorController
    ActuatorController::update();
}

void executeCommand(String command) {
    Serial.print("Executing command: ");
    Serial.println(command);

    if (command.equalsIgnoreCase("help")) {
        printHelp();
    } else if (command.startsWith("test ")) {
        String testCommand = command.substring(5);  // Remove "test " from the beginning
        ActuatorController::executeActuator(testCommand);
    } else if (command.equalsIgnoreCase("tests")) {
        Serial.println("Starting tests...");
        stateMachine.startTests(airPump, drainPump, stirringMotor, nutrientPump, basePump, heatingPlate, ledGrowLight,
                                waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
    } else if (command.startsWith("drain")) {
        Serial.println("Starting drain...");
        int spaceIndex1 = command.indexOf(' ');
        int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
        int rate = command.substring(spaceIndex1 + 1, spaceIndex2).toInt();
        int duration = command.substring(spaceIndex2 + 1).toInt();
        stateMachine.startDrain(drainPump, rate, duration);
    } else if (command.equalsIgnoreCase("stop")) {
        Serial.println("Stopping all...");
        stateMachine.stopAll(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
    } else if (command.startsWith("mix")) {
        Serial.println("Starting mix...");
        int spaceIndex = command.indexOf(' ');
        int speed = command.substring(spaceIndex + 1).toInt();
        stateMachine.startMix(stirringMotor, speed);
    } else if (command.startsWith("fermentation")) {
        Serial.println("Starting fermentation...");
        int spaceIndex1 = command.indexOf(' ');
        int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
        int spaceIndex3 = command.indexOf(' ', spaceIndex2 + 1);
        int spaceIndex4 = command.indexOf(' ', spaceIndex3 + 1);
        int spaceIndex5 = command.indexOf(' ', spaceIndex4 + 1);
        int spaceIndex6 = command.indexOf(' ', spaceIndex5 + 1);
        int spaceIndex7 = command.indexOf(' ', spaceIndex6 + 1);
        int spaceIndex8 = command.indexOf(' ', spaceIndex7 + 1);

        float tempSetpoint = command.substring(spaceIndex1 + 1, spaceIndex2).toFloat();
        float phSetpoint = command.substring(spaceIndex2 + 1, spaceIndex3).toFloat();
        float doSetpoint = command.substring(spaceIndex3 + 1, spaceIndex4).toFloat();
        float nutrientConc = command.substring(spaceIndex4 + 1, spaceIndex5).toFloat();
        float baseConc = command.substring(spaceIndex5 + 1, spaceIndex6).toFloat();
        int duration = command.substring(spaceIndex6 + 1, spaceIndex7).toInt();
        String experimentName = command.substring(spaceIndex7 + 1, spaceIndex8);
        String comment = command.substring(spaceIndex8 + 1);

        stateMachine.startFermentation(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                                       waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                                       tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, experimentName, comment);
    } else if (command == "test_temp_pid") {
        pidController.startTest("temp");
    } else if (command == "test_ph_pid") {
        pidController.startTest("ph");
    } else if (command == "test_do_pid") {
        pidController.startTest("do");
    } else if (command == "stop_pid_test") {
        pidController.stopTest();
    } else {
        Serial.println("Unknown command. Type 'help' for a list of available commands.");
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
    Serial.println("stop_pid_test - Stop current PID test");
}