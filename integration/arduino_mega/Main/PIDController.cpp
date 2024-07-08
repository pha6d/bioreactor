// PIDController.cpp
#include "PIDController.h"
#include <Arduino.h>

PIDController::PIDController() :
    tempPID(&tempInput, &tempOutput, &tempSetpoint, 0, 0, 0, DIRECT),
    phPID(&phInput, &phOutput, &phSetpoint, 0, 0, 0, DIRECT),
    doPID(&doInput, &doOutput, &doSetpoint, 0, 0, 0, DIRECT),
    tempSetpoint(25.0), phSetpoint(7.0), doSetpoint(6.0),
    testRunning(false), lastTestUpdate(0)
{
    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);
}

void PIDController::initialize(double tempKp, double tempKi, double tempKd,
                               double phKp, double phKi, double phKd,
                               double doKp, double doKi, double doKd) {
    tempPID.SetTunings(tempKp, tempKi, tempKd);
    phPID.SetTunings(phKp, phKi, phKd);
    doPID.SetTunings(doKp, doKi, doKd);
}

void PIDController::updateAll(double tempInput, double phInput, double doInput) {
    this->tempInput = tempInput;
    this->phInput = phInput;
    this->doInput = doInput;
    
    tempPID.Compute();
    phPID.Compute();
    doPID.Compute();
}

void PIDController::startTest(const String& type) {
    testRunning = true;
    testType = type;
    lastTestUpdate = 0; // Force une mise à jour immédiate
}

void PIDController::updateTest() {
    if (!testRunning) return;

    unsigned long currentMillis = millis();
    if (currentMillis - lastTestUpdate >= TEST_INTERVAL) {
        lastTestUpdate = currentMillis;

        if (testType == "temp") {
            tempPID.Compute();
            Serial.print("Temperature - Setpoint: ");
            Serial.print(tempSetpoint);
            Serial.print(", Input: ");
            Serial.print(tempInput);
            Serial.print(", Output: ");
            Serial.println(tempOutput);
        } else if (testType == "ph") {
            phPID.Compute();
            Serial.print("pH - Setpoint: ");
            Serial.print(phSetpoint);
            Serial.print(", Input: ");
            Serial.print(phInput);
            Serial.print(", Output: ");
            Serial.println(phOutput);
        } else if (testType == "do") {
            doPID.Compute();
            Serial.print("Dissolved Oxygen - Setpoint: ");
            Serial.print(doSetpoint);
            Serial.print(", Input: ");
            Serial.print(doInput);
            Serial.print(", Output: ");
            Serial.println(doOutput);
        }
    }
}

void PIDController::stopTest() {
    testRunning = false;
}