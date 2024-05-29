/*
 * LEDGrowLight.cpp
 * This file provides the implementation of the LEDGrowLight class defined in LEDGrowLight.h.
 * The class controls a LED grow light using a relay.
 */

#include "LEDGrowLight.h"

// Constructor for LEDGrowLight
LEDGrowLight::LEDGrowLight(int relayPin, const char* id) : _relayPin(relayPin), _id(id) {
    pinMode(_relayPin, OUTPUT); // Set relay pin as output
    digitalWrite(_relayPin, LOW); // Ensure relay is off initially
}

// Method to control the LED grow light
void LEDGrowLight::control(bool state, int value) {
    if (state) {
        digitalWrite(_relayPin, HIGH); // Turn on the relay
        Serial.print(_id);
        Serial.println(" is ON");
    } else {
        digitalWrite(_relayPin, LOW); // Turn off the relay
        Serial.print(_id);
        Serial.println(" is OFF");
    }
}
