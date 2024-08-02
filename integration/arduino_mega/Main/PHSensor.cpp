/*
 * PHSensor.cpp
 * This file provides the implementation of the PHSensor class defined in PHSensor.h.
 * The class reads pH value from a pH sensor and performs calibration.
 */

#include "PHSensor.h"
#include "Logger.h"

// Constructor for PHSensor
PHSensor::PHSensor(int pin, const char* name) : _pin(pin), _name(name) {}

// Method to initialize the pH sensor
void PHSensor::begin() {
    _ph.begin();
    Logger::log(LogLevel::INFO, String(_name) + " initialized");
}

// Method to read the pH value from the sensor
float PHSensor::readValue() {
    _voltage = analogRead(_pin) / 1024.0 * 5000; // Convert analog reading to millivolts
    return _ph.readPH(_voltage, _temperature); // Calculate pH value with temperature compensation
}

// Method to perform pH calibration
void PHSensor::calibrate(float voltage, float temperature) {
    _ph.calibration(voltage, temperature); // Perform calibration using the DFRobot_PH library
}
