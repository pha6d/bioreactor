#include "ActuatorController.h"
#include <Arduino.h>

// External actuator declarations
extern PeristalticPump nutrientPump;
extern PeristalticPump basePump;
extern DCPump airPump;
extern DCPump drainPump;
extern StirringMotor stirringMotor;
extern HeatingPlate heatingPlate;
extern LEDGrowLight ledGrowLight;
extern bool stopFlag;

bool ActuatorController::testRunning = false;
String ActuatorController::currentActuator = "";
unsigned long ActuatorController::testStartTime = 0;
int ActuatorController::testDuration = 0;

void ActuatorController::runActuator(const String& actuatorName, float value, int duration) {
    currentActuator = actuatorName;
    testRunning = true;
    testStartTime = millis();
    testDuration = duration;

    if (actuatorName == "basePump") {
        basePump.control(true, value);
    } else if (actuatorName == "nutrientPump") {
        nutrientPump.control(true, value);
    } else if (actuatorName == "airPump") {
        airPump.control(true, static_cast<int>(value));
    } else if (actuatorName == "drainPump") {
        drainPump.control(true, static_cast<int>(value));
    } else if (actuatorName == "stirringMotor") {
        stirringMotor.control(true, static_cast<int>(value));
    } else if (actuatorName == "heatingPlate") {
        heatingPlate.control(true, static_cast<int>(value));
    } else if (actuatorName == "ledGrowLight") {
        ledGrowLight.control(true, static_cast<int>(value));
    } else {
        Serial.println("Unknown actuator: " + actuatorName);
        testRunning = false;
        return;
    }

    logActuatorOperation(actuatorName, value, duration);
}

void ActuatorController::executeActuator(const String& command) {
    int firstSpace = command.indexOf(' ');
    int secondSpace = command.indexOf(' ', firstSpace + 1);
    
    if (firstSpace != -1 && secondSpace != -1) {
        String actuatorName = command.substring(0, firstSpace);
        float value = command.substring(firstSpace + 1, secondSpace).toFloat();
        int duration = command.substring(secondSpace + 1).toInt();
        
        runActuator(actuatorName, value, duration);
    } else {
        Serial.println("Invalid command format. Use: <actuatorName> <value> <duration>");
    }
}

void ActuatorController::update() {
    if (testRunning) {
        if (millis() - testStartTime >= testDuration * 1000UL || stopFlag) {
            stopAllActuators();
            Serial.println("Test of " + currentActuator + " completed or stopped");
        }
    }
}

void ActuatorController::stopAllActuators() {
    basePump.control(false, 0);
    nutrientPump.control(false, 0);
    airPump.control(false, 0);
    drainPump.control(false, 0);
    stirringMotor.control(false, 0);
    heatingPlate.control(false);
    ledGrowLight.control(false);
    testRunning = false;
    currentActuator = "";
    Serial.println("All actuators stopped");
}

bool ActuatorController::isTestRunning() {
    return testRunning;
}

void ActuatorController::logActuatorOperation(const String& actuatorName, float value, int duration) {
    Serial.println("Running " + actuatorName + " at value " + String(value) + " for " + String(duration) + " seconds");
}

void ActuatorController::emergencyStop() {
    stopAllActuators();
    Serial.println("Emergency stop initiated. All actuators stopped.");
}

// The following methods are not used in the current implementation but are kept for potential future use
void ActuatorController::runPeristalticPump(PeristalticPump& pump, float flowRate, int duration) {
    logActuatorOperation(pump.getName(), flowRate, duration);
    pump.control(true, flowRate);
    delay(duration * 1000);
    pump.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runDCPump(DCPump& pump, int speed, int duration) {
    logActuatorOperation(pump.getName(), speed, duration);
    pump.control(true, speed);
    delay(duration * 1000);
    pump.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runStirringMotor(StirringMotor& motor, int speed, int duration) {
    logActuatorOperation("Stirring Motor", speed, duration);
    motor.control(true, speed);
    delay(duration * 1000);
    motor.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runHeatingPlate(HeatingPlate& heatingPlate, int temperature, int duration) {
    logActuatorOperation("Heating Plate", temperature, duration);
    heatingPlate.control(true, temperature);
    delay(duration * 1000);
    heatingPlate.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runLEDGrowLight(LEDGrowLight& light, int intensity, int duration) {
    logActuatorOperation("LED Grow Light", intensity, duration);
    light.control(true, intensity);
    delay(duration * 1000);
    light.control(false, 0);
    Serial.println("Operation completed");
}