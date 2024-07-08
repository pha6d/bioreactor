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

void ActuatorController::runActuator(const String& actuatorName, float value, int duration) {
    if (actuatorName == "basePump") {
        runPeristalticPump(basePump, value, duration);
    } else if (actuatorName == "nutrientPump") {
        runPeristalticPump(nutrientPump, value, duration);
    } else if (actuatorName == "airPump") {
        runDCPump(airPump, static_cast<int>(value), duration);
    } else if (actuatorName == "drainPump") {
        runDCPump(drainPump, static_cast<int>(value), duration);
    } else if (actuatorName == "stirringMotor") {
        runStirringMotor(stirringMotor, static_cast<int>(value), duration);
    } else if (actuatorName == "heatingPlate") {
        runHeatingPlate(heatingPlate, static_cast<int>(value), duration);
    } else if (actuatorName == "ledGrowLight") {
        runLEDGrowLight(ledGrowLight, static_cast<int>(value), duration);
    } else {
        Serial.println("Unknown actuator: " + actuatorName);
    }
}

void ActuatorController::executeActuator(const String& command) {
    // Parsing the command
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

void ActuatorController::logActuatorOperation(const String& actuatorName, float value, int duration) {
    Serial.println("Running " + actuatorName + " at value " + String(value) + " for " + String(duration) + " seconds");
}