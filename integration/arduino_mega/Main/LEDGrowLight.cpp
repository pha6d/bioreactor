/*
 * LEDGrowLight.cpp
 * This file provides the implementation of the LEDGrowLight class defined in LEDGrowLight.h.
 * The class controls a LED grow light using a relay and potentially PWM for intensity control.
 */

#include "LEDGrowLight.h"

// Constructor for LEDGrowLight
LEDGrowLight::LEDGrowLight(int relayPin, const char* id)
    : _relayPin(relayPin), _id(id), status(false), intensity(0) {
    pinMode(_relayPin, OUTPUT); // Set relay pin as output
    digitalWrite(_relayPin, LOW); // Ensure relay is off initially
}

// Method to control the LED grow light
void LEDGrowLight::control(bool state, int value) {
    if (state) {
        intensity = constrain(value, 0, 100); // Constrain intensity to 0-100%
        digitalWrite(_relayPin, HIGH); // Turn on the relay
        status = true; // Set the status to on
        Serial.print(_id); // Print the LED grow light identifier
        Serial.print(" is ON with intensity: ");
        Serial.print(intensity);
        Serial.println("%");
    } else {
        digitalWrite(_relayPin, LOW); // Turn off the relay
        status = false; // Set the status to off
        intensity = 0; // Reset intensity to 0
        Serial.print(_id); // Print the LED grow light identifier
        Serial.println(" is OFF");
    }
}

// Method to check if the LED grow light is on
bool LEDGrowLight::isOn() const {
    return status;
}