/*
 * PeristalticPump.cpp
 * This file provides the implementation of the PeristalticPump class defined in PeristalticPump.h.
 * The class controls a peristaltic pump using a DAC (MCP4725) and a relay.
 */

#include "PeristalticPump.h"

// Constructor for PeristalticPump
PeristalticPump::PeristalticPump(uint8_t dacAddress, int relayPin, float maxFlowRate, const char* id)
    : _dacAddress(dacAddress), _relayPin(relayPin), _maxFlowRate(maxFlowRate), _id(id), status(false) {
}

// Initializes the peristaltic pump by setting up the relay pin and the DAC
void PeristalticPump::begin() {
    pinMode(_relayPin, OUTPUT);      // Set relay pin as output
    digitalWrite(_relayPin, LOW);    // Ensure relay is off initially
    _dac.begin(_dacAddress);         // Initialize the DAC with its I2C address
    Serial.println("PeristalticPump initialized");
}

// Controls the pump's state and flow rate
void PeristalticPump::control(bool state, int value) {
    if (state && value > 0) {
        uint16_t dacValue = flowRateToDAC(value);  // Convert flow rate to DAC value
        _dac.setVoltage(dacValue, false);          // Set DAC voltage
        digitalWrite(_relayPin, HIGH);             // Turn on the relay
        status = true;                             // Set the status to on
        Serial.print(_id);
        Serial.print(" is ON with flow rate: ");
        Serial.println(value);
    } else {
        _dac.setVoltage(0, false);                 // Set DAC voltage to 0
        digitalWrite(_relayPin, LOW);              // Turn off the relay
        status = false;                            // Set the status to off
        Serial.print(_id);
        Serial.println(" is OFF");
    }
}

// Method to check if the pump is on
bool PeristalticPump::isOn() const {
    return status;
}

// Converts flow rate in ml/min to DAC value
uint16_t PeristalticPump::flowRateToDAC(float flowRate) {
    float proportion = flowRate / _maxFlowRate;       // Calculate proportion of max flow rate
    return static_cast<uint16_t>(proportion * 4095);  // Convert to DAC value (0-4095)
}
