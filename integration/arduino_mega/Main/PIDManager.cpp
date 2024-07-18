#include "PIDManager.h"
#include <Arduino.h>

PIDManager::PIDManager(StirringMotor& stirringMotor)
    : tempPID(&tempInput, &tempOutput, &tempSetpoint, 0, 0, 0, DIRECT),
      phPID(&phInput, &phOutput, &phSetpoint, 0, 0, 0, DIRECT),
      doPID(&doInput, &doOutput, &doSetpoint, 0, 0, 0, DIRECT),
      stirringMotor(stirringMotor),
      testRunning(false), lastTestUpdate(0), running(false), stopFlag(false)
{
    // Set PID controllers to automatic mode
    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);
}

void PIDManager::initialize(double tempKp, double tempKi, double tempKd,
                            double phKp, double phKi, double phKd,
                            double doKp, double doKi, double doKd) {
    // Set PID tuning parameters
    tempPID.SetTunings(tempKp, tempKi, tempKd);
    phPID.SetTunings(phKp, phKi, phKd);
    doPID.SetTunings(doKp, doKi, doKd);
}

void PIDManager::updateAll(double tempInput, double phInput, double doInput) {
    // Check if PID manager is running
    if (!running || stopFlag) {
        return;
    }

    // Update input values
    this->tempInput = tempInput;
    this->phInput = phInput;
    this->doInput = doInput;
    
    // Compute new PID outputs
    tempPID.Compute();
    phPID.Compute();
    doPID.Compute();
}

double PIDManager::getTemperatureOutput() const { return tempOutput; }
double PIDManager::getPHOutput() const { return phOutput; }
double PIDManager::getDOOutput() const { return doOutput; }

void PIDManager::setTemperatureSetpoint(double setpoint) { tempSetpoint = setpoint; }
void PIDManager::setPHSetpoint(double setpoint) { phSetpoint = setpoint; }
void PIDManager::setDOSetpoint(double setpoint) { doSetpoint = setpoint; }

void PIDManager::startTest(const String& type) {
    testRunning = true;
    testType = type;
    lastTestUpdate = 0; // Force an immediate update
    Serial.println("Starting PID test for " + type);
}

void PIDManager::updateTest() {
    if (!testRunning) return;

    unsigned long currentMillis = millis();
    if (currentMillis - lastTestUpdate >= TEST_INTERVAL) {
        lastTestUpdate = currentMillis;

        if (testType == "temp") {
            tempPID.Compute();
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

void PIDManager::stopTest() {
    testRunning = false;
    Serial.println("PID test stopped");
}

bool PIDManager::isTestRunning() const {
    return testRunning;
}

void PIDManager::adjustStirringSpeed(int minSpeed) {
    // Calculate the maximum PID output
    double maxOutput = max(max(abs(tempOutput), abs(phOutput)), abs(doOutput));
    
    // Map the maximum output to the stirring motor speed range
    int pidSpeed = map(maxOutput, 0, 255, stirringMotor.getMinRPM(), stirringMotor.getMaxRPM());
    
    // Choose the maximum between PID-calculated speed and minimum speed
    int finalSpeed = max(pidSpeed, minSpeed);
    
    // Control the stirring motor
    stirringMotor.control(true, finalSpeed);
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