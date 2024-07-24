#include "PIDTestProgram.h"
#include <Arduino.h>

PIDTestProgram::PIDTestProgram(PIDManager& pidManager)
    : pidManager(pidManager), running(false), paused(false) {}


void PIDTestProgram::configure(const String& pidType, double setpoint) {
    currentPIDType = pidType;
    startTime = millis();

    if (pidType == "temp") {
        pidManager.startTemperaturePID(setpoint);
    } else if (pidType == "ph") {
        pidManager.startPHPID(setpoint);
    } else if (pidType == "do") {
        pidManager.startDOPID(setpoint);
    } else {
        Serial.println("Invalid PID type for test");
        running = false;
        return;
    }

    Serial.println("Starting PID test for " + pidType + " with setpoint " + String(setpoint));
}

void PIDTestProgram::begin() {
    running = true;
    paused = false;
}

void PIDTestProgram::update() {
    if (!running || paused) return;

    if (millis() - startTime >= TEST_DURATION) {
        stop();
        return;
    }

    if (currentPIDType == "temp") {
        pidManager.updateTemperaturePID(waterTempSensor->readValue());
        heatingPlate->controlWithPID(pidManager.getTemperatureOutput());
    } else if (currentPIDType == "ph") {
        pidManager.updatePHPID(phSensor->readValue());
    } else if (currentPIDType == "do") {
        pidManager.updateDOPID(oxygenSensor->readValue());
        airPump->control(true, pidManager.getDOOutput());
    }

    pidManager.adjustStirringSpeed(390); // Exemple de vitesse minimale
}

void PIDTestProgram::pause() {
    if (running && !paused) {
        paused = true;
        pidManager.pauseAllPID();
        // Arrêter les actuateurs pertinents
        if (currentPIDType == "temp") {
            heatingPlate->control(false);
        } else if (currentPIDType == "do") {
            airPump->control(false, 0);
        }
        Serial.println("PID test paused");
    }
}

void PIDTestProgram::resume() {
    if (running && paused) {
        paused = false;
        pidManager.resumeAllPID();
        // Redémarrer les actuateurs pertinents
        if (currentPIDType == "temp") {
            heatingPlate->control(true);
        } else if (currentPIDType == "do") {
            airPump->control(true, pidManager.getDOOutput());
        }
        Serial.println("PID test resumed");
    }
}

void PIDTestProgram::stop() {
    if (!running) return;

    if (currentPIDType == "temp") {
        pidManager.stopTemperaturePID();
    } else if (currentPIDType == "ph") {
        pidManager.stopPHPID();
    } else if (currentPIDType == "do") {
        pidManager.stopDOPID();
    }

    running = false;
    paused = false;
    Serial.println("PID test stopped for " + currentPIDType);
}

bool PIDTestProgram::isRunning() const {
    return running;
}