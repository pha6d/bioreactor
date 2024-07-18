/*
 * FermentationProgram.cpp
 * This file implements the FermentationProgram class defined in FermentationProgram.h.
 * It manages the fermentation process, including sensor readings, PID control, and actuator adjustments.
 */

#include "FermentationProgram.h"

FermentationProgram::FermentationProgram(PIDManager& pidManager)
    : pidManager(pidManager), running(false) {}

void FermentationProgram::begin(DCPump& airPump, DCPump& drainPump, 
               PeristalticPump& nutrientPump, PeristalticPump& basePump, 
               StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
               PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, 
               PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
               OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
               float tempSetpoint, float phSetpoint, float doSetpoint, 
               float nutrientConc, float baseConc, int duration, 
               const String& experimentName, const String& comment) {
    
    // Store actuator pointers
    this->airPump = &airPump;
    this->drainPump = &drainPump;
    this->nutrientPump = &nutrientPump;
    this->basePump = &basePump;
    this->stirringMotor = &stirringMotor;
    this->heatingPlate = &heatingPlate;
    this->ledGrowLight = &ledGrowLight;

    // Store sensor pointers
    this->waterTempSensor = &waterTempSensor;
    this->airTempSensor = &airTempSensor;
    this->phSensor = &phSensor;
    this->turbiditySensor = &turbiditySensor;
    this->oxygenSensor = &oxygenSensor;
    this->airFlowSensor = &airFlowSensor;

    // Store setpoints and other parameters
    this->tempSetpoint = tempSetpoint;
    this->phSetpoint = phSetpoint;
    this->doSetpoint = doSetpoint;
    this->nutrientConc = nutrientConc;
    this->baseConc = baseConc;
    this->duration = duration;
    this->experimentName = experimentName;
    this->comment = comment;

    // Initialize PID setpoints
    pidManager.setTemperatureSetpoint(tempSetpoint);
    pidManager.setPHSetpoint(phSetpoint);
    pidManager.setDOSetpoint(doSetpoint);

    // Start the fermentation process
    startTime = millis();
    running = true;

    // Initialize actuators
    airPump.control(true, 50);  // Example: Start air pump at 50% speed
    stirringMotor.control(true, 100);  // Example: Start stirring at 100 RPM

    Serial.println("Fermentation started: " + experimentName);
    Serial.println("Comment: " + comment);
}

void FermentationProgram::update() {
    if (!running) return;

    if (stopFlag) {
        stop();
        return;
    }

    updateSensors();
    applyPIDOutputs();
    checkCompletion();
}

void FermentationProgram::updateSensors() {
    double tempValue = waterTempSensor->readValue();
    double phValue = phSensor->readValue();
    double doValue = oxygenSensor->readValue();
    
    pidManager.updateAll(tempValue, phValue, doValue);
}

void FermentationProgram::applyPIDOutputs() {
    double tempOutput = pidManager.getTemperatureOutput();
    double phOutput = pidManager.getPHOutput();
    double doOutput = pidManager.getDOOutput();

    // Apply temperature control
    heatingPlate->controlWithPID(tempOutput);

    // Apply pH control
    if (phOutput > 0) {
        // If pH is too low, add base
        basePump->control(true, static_cast<int>(phOutput));
    } else {
        basePump->control(false, 0);
    }

    // Apply dissolved oxygen control
    airPump->control(true, static_cast<int>(doOutput));

    // Adjust stirring speed based on the maximum of the three outputs
    int stirringSpeed = static_cast<int>(max(max(abs(tempOutput), abs(phOutput)), abs(doOutput)));
    stirringMotor->control(true, stirringSpeed);
}

void FermentationProgram::checkCompletion() {
    if (millis() - startTime >= static_cast<unsigned long>(duration) * 1000UL) {
        stop();
        Serial.println("Fermentation completed.");
    }
}

bool FermentationProgram::isRunning() const {
    return running;
}

void FermentationProgram::stop() {
    running = false;

    // Stop all actuators
    airPump->control(false, 0);
    drainPump->control(false, 0);
    nutrientPump->control(false, 0);
    basePump->control(false, 0);
    stirringMotor->control(false, 0);
    heatingPlate->control(false);
    ledGrowLight->control(false);

    Serial.println("Fermentation stopped.");
}