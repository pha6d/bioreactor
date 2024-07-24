/*
 * FermentationProgram.cpp
 * This file implements the FermentationProgram class defined in FermentationProgram.h.
 * It controls the fermentation process of the bioreactor.
 */

#include "FermentationProgram.h"

FermentationProgram::FermentationProgram(PIDManager& pidManager, VolumeManager& volumeManager)
    : pidManager(pidManager), volumeManager(volumeManager), running(false), paused(false), totalPauseTime(0) {}

void FermentationProgram::begin() {
    // Initialisation de base
    running = true;
    paused = false;
    startTime = millis();
    totalPauseTime = 0;
}

void FermentationProgram::configure(DCPump& airPump, DCPump& drainPump,
                                    PeristalticPump& nutrientPump, PeristalticPump& basePump,
                                    StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                                    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                                    PHSensor& phSensor, TurbiditySensor& turbiditySensor,
                                    OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
                                    float tempSetpoint, float phSetpoint, float doSetpoint,
                                    float nutrientConc, float baseConc, int duration,
                                    const String& experimentName, const String& comment) {
    
    // Store actuator and sensor pointers
    this->airPump = &airPump;
    this->drainPump = &drainPump;
    this->nutrientPump = &nutrientPump;
    this->basePump = &basePump;
    this->stirringMotor = &stirringMotor;
    this->heatingPlate = &heatingPlate;
    this->ledGrowLight = &ledGrowLight;
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
    pidManager.startTemperaturePID(tempSetpoint);
    pidManager.startPHPID(phSetpoint);
    pidManager.startDOPID(doSetpoint);

    // Start the fermentation process
    startTime = millis();
    running = true;
    paused = false;
    totalPauseTime = 0;

    // Initialize actuators
    airPump.control(true, 50);  // Example: Start air pump at 50% speed
    stirringMotor.control(true, 100);  // Example: Start stirring at 100 RPM

    Serial.println("Fermentation started: " + experimentName);
    Serial.println("Comment: " + comment);
}

void FermentationProgram::update() {
    if (!running || paused) return;

    updateSensors();
    applyPIDOutputs();
    updateVolume();
    checkCompletion();
}

void FermentationProgram::pause() {
    if (!running || paused) return;

    paused = true;
    pauseStartTime = millis();

    // Maintain minimum stirring speed and temperature
    stirringMotor->control(true, 30);  // Example: Set to 30% speed
    heatingPlate->controlWithPID(pidManager.getTemperatureOutput());

    // Stop other actuators
    airPump->control(false, 0);
    drainPump->control(false, 0);
    nutrientPump->control(false, 0);
    basePump->control(false, 0);

    Serial.println("Fermentation paused");
}

void FermentationProgram::resume() {
    if (!running || !paused) return;

    paused = false;
    totalPauseTime += millis() - pauseStartTime;

    // Resume normal operation of actuators
    airPump->control(true, 50);  // Example: Resume air pump at 50% speed
    applyPIDOutputs();  // This will resume normal control of all actuators

    Serial.println("Fermentation resumed");
}

void FermentationProgram::stop() {
    if (!running) return;

    running = false;
    paused = false;

    // Stop all actuators
    airPump->control(false, 0);
    drainPump->control(false, 0);
    nutrientPump->control(false, 0);
    basePump->control(false, 0);
    stirringMotor->control(false, 0);
    heatingPlate->control(false);
    ledGrowLight->control(false);

    // Stop PID controllers
    pidManager.stopTemperaturePID();
    pidManager.stopPHPID();
    pidManager.stopDOPID();

    Serial.println("Fermentation stopped");
}

bool FermentationProgram::isRunning() const {
    return running;
}

void FermentationProgram::updateSensors() {
    double tempValue = waterTempSensor->readValue();
    double phValue = phSensor->readValue();
    double doValue = oxygenSensor->readValue();
    
    pidManager.updateTemperaturePID(tempValue);
    pidManager.updatePHPID(phValue);
    pidManager.updateDOPID(doValue);
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
    stirringMotor->control(true, max(stirringSpeed, 100)); // Use a minimum speed of 100
}

void FermentationProgram::updateVolume() {
    volumeManager.updateVolume();
}

void FermentationProgram::checkCompletion() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime - totalPauseTime;
    if (elapsedTime >= static_cast<unsigned long>(duration) * 1000UL) {
        stop();
        Serial.println("Fermentation completed");
    }
}