#include "PIDManager.h"
#include <Arduino.h>

// Constructor
PIDManager::PIDManager(StirringMotor* stirringMotor, HeatingPlate* heatingPlate)
    : tempPID(&tempInput, &tempOutput, &tempSetpoint, 0, 0, 0, DIRECT),
      phPID(&phInput, &phOutput, &phSetpoint, 0, 0, 0, DIRECT),
      doPID(&doInput, &doOutput, &doSetpoint, 0, 0, 0, DIRECT),
      stirringMotor(stirringMotor),
      heatingPlate(heatingPlate),
      testRunning(false), lastTestUpdate(0),
      tempPIDRunning(false), phPIDRunning(false), doPIDRunning(false)
{
    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);
}

// Initialize PID controllers
void PIDManager::initialize(double tempKp, double tempKi, double tempKd,
                            double phKp, double phKi, double phKd,
                            double doKp, double doKi, double doKd) {
    tempPID.SetTunings(tempKp, tempKi, tempKd);
    phPID.SetTunings(phKp, phKi, phKd);
    doPID.SetTunings(doKp, doKi, doKd);
}

// Update all PID controllers
void PIDManager::updateAll(double tempInput, double phInput, double doInput) {
    checkStopFlag();
    if (stopFlag) {
        tempOutput = phOutput = doOutput = 0;
        return;
    }

    this->tempInput = tempInput;
    this->phInput = phInput;
    this->doInput = doInput;
    
    if (tempPIDRunning) tempPID.Compute();
    if (phPIDRunning) phPID.Compute();
    if (doPIDRunning) doPID.Compute();
}

// Set setpoints
void PIDManager::setTemperatureSetpoint(double setpoint) { tempSetpoint = setpoint; }
void PIDManager::setPHSetpoint(double setpoint) { phSetpoint = setpoint; }
void PIDManager::setDOSetpoint(double setpoint) { doSetpoint = setpoint; }

// Get outputs
double PIDManager::getTemperatureOutput() const { return tempOutput; }
double PIDManager::getPHOutput() const { return phOutput; }
double PIDManager::getDOOutput() const { return doOutput; }

// Start PID test
void PIDManager::startTest(const String& type) {
    testRunning = true;
    testType = type;
    lastTestUpdate = 0;
    if (type == "temp") {
        tempSetpoint = 37.0; // A realistic setpoint for a bioreactor
        tempPID.SetMode(AUTOMATIC);
    } else if (type == "ph") {
        phSetpoint = 7.0; // Example setpoint for pH
        phPID.SetMode(AUTOMATIC);
    } else if (type == "do") {
        doSetpoint = 6.0; // Example setpoint for dissolved oxygen (mg/L)
        doPID.SetMode(AUTOMATIC);
    }
    Serial.println("Starting PID test for " + type + " with setpoint " + String(tempSetpoint));
}

// Update PID test
void PIDManager::updateTest() {
    if (!testRunning) return;

    checkStopFlag();
    if (!testRunning) return;

    unsigned long currentMillis = millis();
    if (currentMillis - lastTestUpdate >= TEST_INTERVAL) {
        lastTestUpdate = currentMillis;

        if (testType == "temp") {
            tempPID.Compute();
            heatingPlate->controlWithPID(tempOutput);
            logPIDValues("Temperature", tempSetpoint, tempInput, tempOutput);
        } else if (testType == "ph") {
            phPID.Compute();
            logPIDValues("pH", phSetpoint, phInput, phOutput);
        } else if (testType == "do") {
            doPID.Compute();
            logPIDValues("Dissolved Oxygen", doSetpoint, doInput, doOutput);
        }
    }
}

// Stop PID test
void PIDManager::stopTest() {
    testRunning = false;
    heatingPlate->control(false, 0);
    Serial.println("PID test stopped");
}

// Check if test is running
bool PIDManager::isTestRunning() const {
    return testRunning;
}

// Adjust stirring speed based on PID outputs
void PIDManager::adjustStirringSpeed(int minSpeed) {
    double maxOutput = max(max(abs(tempOutput), abs(phOutput)), abs(doOutput));
    int pidSpeed = map(maxOutput, 0, 255, stirringMotor->getMinRPM(), stirringMotor->getMaxRPM());
    int finalSpeed = max(pidSpeed, minSpeed);
    stirringMotor->control(true, finalSpeed);
}

// Log PID values
void PIDManager::logPIDValues(const String& type, double setpoint, double input, double output) {
    Serial.print(type);
    Serial.print(" - Setpoint: ");
    Serial.print(setpoint);
    Serial.print(", Input: ");
    Serial.print(input);
    Serial.print(", Output: ");
    Serial.println(output);
}

// Check stop flag
void PIDManager::checkStopFlag() {
    if (stopFlag) {
        stopTest();
        tempOutput = phOutput = doOutput = 0;
        heatingPlate->control(false, 0);
    }
}

// New methods for individual PID control
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
    tempInput = input;
    tempPID.Compute();
    heatingPlate->controlWithPID(tempOutput);
    logPIDValues("Temperature", tempSetpoint, tempInput, tempOutput);
}

void PIDManager::updatePHPID(double input) {
    if (!phPIDRunning) return;
    phInput = input;
    phPID.Compute();
    logPIDValues("pH", phSetpoint, phInput, phOutput);
}

void PIDManager::updateDOPID(double input) {
    if (!doPIDRunning) return;
    doInput = input;
    doPID.Compute();
    logPIDValues("Dissolved Oxygen", doSetpoint, doInput, doOutput);
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