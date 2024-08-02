// PIDManager.cpp
#include "PIDManager.h"
#include "Logger.h"
#include <Arduino.h>

PIDManager::PIDManager()
    : tempPID(&tempInput, &tempOutput, &tempSetpoint, 0, 0, 0, DIRECT),
      phPID(&phInput, &phOutput, &phSetpoint, 0, 0, 0, DIRECT),
      doPID(&doInput, &doOutput, &doSetpoint, 0, 0, 0, DIRECT),
      tempPIDRunning(false), phPIDRunning(false), doPIDRunning(false),
      lastTempUpdateTime(0), lastPHUpdateTime(0), lastDOUpdateTime(0),
      minStirringSpeed(0)
{
    tempPID.SetOutputLimits(0, 100);
    phPID.SetOutputLimits(0, 100);
    doPID.SetOutputLimits(0, 100);
    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);
}

void PIDManager::initialize(double tempKp, double tempKi, double tempKd,
                            double phKp, double phKi, double phKd,
                            double doKp, double doKi, double doKd) {
    tempPID.SetTunings(tempKp, tempKi, tempKd);
    phPID.SetTunings(phKp, phKi, phKd);
    doPID.SetTunings(doKp, doKi, doKd);
}

void PIDManager::updateAllPIDControllers() {
    unsigned long currentTime = millis();
    bool pidUpdated = false;
    if (tempPIDRunning && currentTime - lastTempUpdateTime >= UPDATE_INTERVAL_TEMP) {
        updateTemperaturePID();
        lastTempUpdateTime = currentTime;
        pidUpdated = true;
    }
    if (phPIDRunning && currentTime - lastPHUpdateTime >= UPDATE_INTERVAL_PH) {
        updatePHPID();
        lastPHUpdateTime = currentTime;
        pidUpdated = true;
    }
    if (doPIDRunning && currentTime - lastDOUpdateTime >= UPDATE_INTERVAL_DO) {
        updateDOPID();
        lastDOUpdateTime = currentTime;
        pidUpdated = true;
    }
    if (pidUpdated) {
        adjustPIDStirringSpeed();
    }
}

void PIDManager::setTemperatureSetpoint(double setpoint) { tempSetpoint = setpoint; }
void PIDManager::setPHSetpoint(double setpoint) { phSetpoint = setpoint; }
void PIDManager::setDOSetpoint(double setpoint) { doSetpoint = setpoint; }

double PIDManager::getTemperatureOutput() const { return tempOutput; } 
double PIDManager::getPHOutput() const { return phOutput; }             
double PIDManager::getDOOutput() const { return doOutput; }

void PIDManager::startTemperaturePID(double setpoint) {
    tempSetpoint = setpoint;
    tempPIDRunning = true;
    Logger::log(LogLevel::INFO, "Temperature PID started with setpoint: " + String(setpoint));
}

void PIDManager::startPHPID(double setpoint) {
    phSetpoint = setpoint;
    phPIDRunning = true;
    Logger::log(LogLevel::INFO, "pH PID started with setpoint: " + String(setpoint));
}

void PIDManager::startDOPID(double setpoint) {
    doSetpoint = setpoint;
    doPIDRunning = true;
    Logger::log(LogLevel::INFO, "DO PID started with setpoint: " + String(setpoint));
}

void PIDManager::adjustPIDStirringSpeed() {
    if (!tempPIDRunning && !phPIDRunning && !doPIDRunning) return;

    double maxOutput = max(max(abs(tempOutput), abs(phOutput)), abs(doOutput));
    int pidSpeed = map(maxOutput, 0, 255, ActuatorController::getStirringMotorMinRPM(), ActuatorController::getStirringMotorMaxRPM());
    int finalSpeed = max(pidSpeed, getMinStirringSpeed());
    finalSpeed = constrain(finalSpeed, ActuatorController::getStirringMotorMinRPM(), ActuatorController::getStirringMotorMaxRPM());
    ActuatorController::runActuator("stirringMotor", finalSpeed, 0);
    
    Logger::log(LogLevel::INFO, "Adjusted stirring motor speed: " + String(finalSpeed));
}

void PIDManager::updateTemperaturePID() {
    if (!tempPIDRunning) return;
    
    tempInput = SensorController::readSensor("waterTempSensor");
    tempPID.Compute();
    int tempOutputPercentage = map(tempOutput, 0, 255, 0, 100);
    ActuatorController::runActuator("heatingPlate", tempOutputPercentage , 0);
    Logger::log(LogLevel::INFO, "Temperature PID update - Setpoint: " + String(tempSetpoint) + ", Input: " + String(tempInput) + ", Output: " + String(tempOutput));
    Logger::logPIDData("Temperature", tempSetpoint, tempInput, tempOutput);
}

void PIDManager::updatePHPID() {
    if (!phPIDRunning) return;
    
    phInput = SensorController::readSensor("phSensor");
    phPID.Compute();
    if (phOutput > 0) {
        ActuatorController::runActuator("basePump", phOutput, 0);
    } else {
        ActuatorController::stopActuator("basePump");
    }
    Logger::log(LogLevel::INFO, "pH PID update - Setpoint: " + String(phSetpoint) + ", Input: " + String(phInput) + ", Output: " + String(phOutput));
    Logger::logPIDData("pH", phSetpoint, phInput, phOutput);
}

void PIDManager::updateDOPID() {
    if (!doPIDRunning) return;

    doInput = SensorController::readSensor("oxygenSensor");
    doPID.Compute();
    ActuatorController::runActuator("airPump", doOutput, 0);
    Logger::log(LogLevel::INFO, "DO PID update - Setpoint: " + String(doSetpoint) + ", Input: " + String(doInput) + ", Output: " + String(doOutput));
    Logger::logPIDData("Dissolved Oxygen", doSetpoint, doInput, doOutput);
}


void PIDManager::stopTemperaturePID() {
    tempPIDRunning = false;
    tempOutput = 0;
    ActuatorController::stopActuator("heatingPlate");
    Logger::log(LogLevel::INFO, "Temperature PID stopped");
}

void PIDManager::stopPHPID() {
    phPIDRunning = false;
    phOutput = 0;
    ActuatorController::stopActuator("basePump");
    Logger::log(LogLevel::INFO, "pH PID stopped");
}

void PIDManager::stopDOPID() {
    doPIDRunning = false;
    doOutput = 0;
    ActuatorController::stopActuator("airPump");
    Logger::log(LogLevel::INFO, "DO PID stopped");
}

void PIDManager::stopAllPID() {
    stopTemperaturePID();
    stopPHPID();
    stopDOPID();
}

void PIDManager::pauseAllPID() {
    tempPID.SetMode(MANUAL);
    phPID.SetMode(MANUAL);
    doPID.SetMode(MANUAL);
}

void PIDManager::resumeAllPID() {
    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);
}

void PIDManager::adjustPIDParameters(const String& pidType, double Kp, double Ki, double Kd) {
    if (pidType == "temperature") {
        tempPID.SetTunings(Kp, Ki, Kd);
    } else if (pidType == "pH") {
        phPID.SetTunings(Kp, Ki, Kd);
    } else if (pidType == "DO") {
        doPID.SetTunings(Kp, Ki, Kd);
    }
}

// A sauvegarder/charger sur le serveur SI BESOIN de plus de data.
void PIDManager::saveParameters(const char* filename) {
    // Implement saving PID parameters to EEPROM or SD card
    Logger::log(LogLevel::INFO, "Saving PID parameters to " + String(filename));
}
void PIDManager::loadParameters(const char* filename) {
    // Implement loading PID parameters from EEPROM or SD card
    Logger::log(LogLevel::INFO, "Loading PID parameters from " + String(filename));
}
