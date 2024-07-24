#include "PIDManager.h"
#include <Arduino.h>
#include <EEPROM.h>

PIDManager::PIDManager(StirringMotor* stirringMotor, HeatingPlate* heatingPlate)
    : tempPID(&tempInput, &tempOutput, &tempSetpoint, 0, 0, 0, DIRECT),
      phPID(&phInput, &phOutput, &phSetpoint, 0, 0, 0, DIRECT),
      doPID(&doInput, &doOutput, &doSetpoint, 0, 0, 0, DIRECT),
      stirringMotor(stirringMotor),
      heatingPlate(heatingPlate),
      tempPIDRunning(false), phPIDRunning(false), doPIDRunning(false),
      lastTempUpdateTime(0), lastPHUpdateTime(0), lastDOUpdateTime(0)
{
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

void PIDManager::updateAll(double tempInput, double phInput, double doInput) {
    this->tempInput = tempInput;
    this->phInput = phInput;
    this->doInput = doInput;
    
    if (tempPIDRunning) tempPID.Compute();
    if (phPIDRunning) phPID.Compute();
    if (doPIDRunning) doPID.Compute();
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
    Serial.println("Temperature PID started with setpoint: " + String(setpoint));
}

void PIDManager::startPHPID(double setpoint) {
    phSetpoint = setpoint;
    phPIDRunning = true;
    Serial.println("pH PID started with setpoint: " + String(setpoint));
}

void PIDManager::startDOPID(double setpoint) {
    doSetpoint = setpoint;
    doPIDRunning = true;
    Serial.println("DO PID started with setpoint: " + String(setpoint));
}

void PIDManager::updateTemperaturePID(double input) {
    if (!tempPIDRunning) return;
    unsigned long currentTime = millis();
    if (currentTime - lastTempUpdateTime >= UPDATE_INTERVAL_TEMP) {
        lastTempUpdateTime = currentTime;
        tempInput = input;
        tempPID.Compute();
        heatingPlate->controlWithPID(tempOutput);
        logPIDValues("Temperature", tempSetpoint, tempInput, tempOutput);
    }
}

void PIDManager::updatePHPID(double input) {
    if (!phPIDRunning) return;
    unsigned long currentTime = millis();
    if (currentTime - lastPHUpdateTime >= UPDATE_INTERVAL_PH) {
        lastPHUpdateTime = currentTime;
        phInput = input;
        phPID.Compute();
        logPIDValues("pH", phSetpoint, phInput, phOutput);
    }
}

void PIDManager::updateDOPID(double input) {
    if (!doPIDRunning) return;
    unsigned long currentTime = millis();
    if (currentTime - lastDOUpdateTime >= UPDATE_INTERVAL_DO) {
        lastDOUpdateTime = currentTime;
        doInput = input;
        doPID.Compute();
        logPIDValues("Dissolved Oxygen", doSetpoint, doInput, doOutput);
    }
}

void PIDManager::stopTemperaturePID() {
    tempPIDRunning = false;
    tempOutput = 0;
    heatingPlate->control(false, 0);
    Serial.println("Temperature PID stopped");
}

void PIDManager::stopPHPID() {
    phPIDRunning = false;
    phOutput = 0;
    Serial.println("pH PID stopped");
}

void PIDManager::stopDOPID() {
    doPIDRunning = false;
    doOutput = 0;
    Serial.println("DO PID stopped");
}

void PIDManager::adjustStirringSpeed(int minSpeed) {
    double maxOutput = max(max(abs(tempOutput), abs(phOutput)), abs(doOutput));
    int pidSpeed = map(maxOutput, 0, 255, stirringMotor->getMinRPM(), stirringMotor->getMaxRPM());
    int finalSpeed = max(pidSpeed, minSpeed);
    stirringMotor->control(true, finalSpeed);
}

void PIDManager::logPIDValues(const String& type, double setpoint, double input, double output) {
    Serial.print(type);
    Serial.print(" - Setpoint: ");
    Serial.print(setpoint);
    Serial.print(", Input: ");
    Serial.print(input);
    Serial.print(", Output: ");
    Serial.println(output);
}

void PIDManager::saveParameters(const char* filename) {
    // Cette fonction devrait être implémentée pour sauvegarder les paramètres PID dans la mémoire EEPROM ou sur une carte SD
    // Pour l'instant, nous allons simplement afficher un message
    Serial.println("Saving PID parameters to " + String(filename));
}

void PIDManager::loadParameters(const char* filename) {
    // Cette fonction devrait être implémentée pour charger les paramètres PID depuis la mémoire EEPROM ou une carte SD
    // Pour l'instant, nous allons simplement afficher un message
    Serial.println("Loading PID parameters from " + String(filename));
}

void PIDManager::pauseAllPID() {
    // Arrêter tous les PID actifs
    tempPID.SetMode(MANUAL);
    phPID.SetMode(MANUAL);
    doPID.SetMode(MANUAL);
}

void PIDManager::resumeAllPID() {
    // Reprendre tous les PID actifs
    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);
}