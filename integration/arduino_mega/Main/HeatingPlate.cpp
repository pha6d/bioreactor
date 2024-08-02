/*
 * HeatingPlate.cpp
 * This file provides the implementation of the HeatingPlate class defined in HeatingPlate.h.
 * The class controls a heating plate using either on/off control or PWM control.
 */

#include "HeatingPlate.h"
#include "Logger.h"

HeatingPlate::HeatingPlate(int relayPin, bool isPWMCapable, const char* name)
    : _relayPin(relayPin), status(false), _isPWMCapable(isPWMCapable), _name(name) {
    pinMode(_relayPin, OUTPUT);
    digitalWrite(_relayPin, LOW);
}

void HeatingPlate::begin() {
    digitalWrite(_relayPin, LOW); // Ensure the heating plate is off at initialization
    Logger::log(LogLevel::INFO, String(_name) + " initialized");
}

void HeatingPlate::control(bool state, int value) {
    if (_isPWMCapable) {
        controlPWM(state ? map(value, 0, 100, 0, 255) : 0);
    } else {
        controlOnOff(state);
    }
}

bool HeatingPlate::isOn() const {
    return status;
}

void HeatingPlate::controlWithPID(double pidOutput) {
    int percentPower = map(pidOutput, 0, 255, 0, 100);
    
    // Add a ramp to avoid abrupt changes
    int powerDiff = percentPower - _lastPercentPower;
    int maxChange = 5; // Limit change to 5% per cycle
    percentPower = _lastPercentPower + constrain(powerDiff, -maxChange, maxChange);
    
    if (_isPWMCapable) {
        controlPWM(map(percentPower, 0, 100, 0, 255));
    } else {
        controlWithCycle(percentPower);
    }
    _lastPercentPower = percentPower;
}

void HeatingPlate::controlPWM(int value) {
    int percentage = map(value, 0, 255, 0, 100);
    if (value > 0) {
        analogWrite(_relayPin, value);
        status = true;
    } else {
        analogWrite(_relayPin, 0);
        status = false;
    }
    Logger::log(LogLevel::INFO, String(_name) + (status ? " is ON with power: " + String(percentage) + "%" : " is OFF"));
}

void HeatingPlate::controlOnOff(bool state) {
    digitalWrite(_relayPin, state ? HIGH : LOW);
    status = state;
    Logger::log(LogLevel::INFO, String(_name) + (status ? " is ON" : " is OFF"));
}

void HeatingPlate::controlWithCycle(double percentPower) {
    dutyCycle = percentPower / 100.0;
    unsigned long now = millis();
    if (now - lastCycleStart >= cycleTime) {
        lastCycleStart = now;
    }
    if (now - lastCycleStart < cycleTime * dutyCycle) {
        digitalWrite(_relayPin, HIGH);
        status = true;
    } else {
        digitalWrite(_relayPin, LOW);
        status = false;
    }
    Logger::log(LogLevel::INFO, String(_name) + " Duty Cycle: " + String(dutyCycle * 100) + "%");
}