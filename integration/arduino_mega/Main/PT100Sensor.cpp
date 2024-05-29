/*
 * PT100Sensor.cpp
 * This file provides the implementation of the PT100Sensor class defined in PT100Sensor.h.
 * The class reads temperature from a PT100 sensor using the MAX31865 amplifier.
 */

#include "PT100Sensor.h"

// Constructor for PT100Sensor
PT100Sensor::PT100Sensor(int csPin, int diPin, int doPin, int clkPin)
    : _csPin(csPin), _diPin(diPin), _doPin(doPin), _clkPin(clkPin), _max31865(csPin) {}

// Method to initialize the PT100 sensor
void PT100Sensor::begin() {
    _max31865.begin(MAX31865_3WIRE); // Initialize sensor for 3-wire configuration
}

// Method to read the temperature value from the sensor
float PT100Sensor::readValue() {
    // Read temperature from the MAX31865 amplifier
    // Parameters: reference resistance value (100.0 for PT100) and the reference resistor value (430.0 ohms)
    return _max31865.temperature(100.0, 430.0);
}
