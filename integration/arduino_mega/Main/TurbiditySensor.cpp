/*
 * TurbiditySensor.cpp
 * This file provides the implementation of the TurbiditySensor class defined in TurbiditySensor.h.
 * The class reads turbidity values from a turbidity sensor.
 */

#include "TurbiditySensor.h"

// Constructor for TurbiditySensor
TurbiditySensor::TurbiditySensor(int pin) : _pin(pin) {}

// Method to initialize the turbidity sensor
void TurbiditySensor::begin() {
    // Nothing specific to initialize for turbidity sensor in this implementation
}

// Method to read the turbidity value from the sensor
float TurbiditySensor::readValue() {
    int sensorValue = analogRead(_pin); // Read the input on the analog pin
    return sensorValue * (5.0 / 1024.0); // Convert to voltage and return
}
