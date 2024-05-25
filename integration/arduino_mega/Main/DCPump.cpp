/*
 * DCPump.cpp
 * This file provides the implementation of the DCPump class defined in DCPump.h.
 * The class controls a DC pump using PWM signals, a relay, and a motor speed controller.
 * It allows for setting the pump speed and turning it on or off.
 */

#include "DCPump.h"

// Constructor for DCPump
DCPump::DCPump(int pwmPin, int relayPin, int minPWM, const char* id) {
    _pwmPin = pwmPin;
    _relayPin = relayPin;
    _minPWM = minPWM;
    _id = id;
    pinMode(_pwmPin, OUTPUT); // Set PWM pin as output
    pinMode(_relayPin, OUTPUT); // Set relay pin as output
}

// Method to control the pump
void DCPump::control(bool state, int value) {
    if (state && value >= _minPWM && value <= 100) {
        int pwmValue = map(value, _minPWM, 100, 26, 255); // Map speed percentage to PWM value
        analogWrite(_pwmPin, pwmValue); // Set the PWM value
        digitalWrite(_relayPin, HIGH); // Turn on the relay
        Serial.print(_id); // Print the pump identifier
        Serial.print(" is ON, Speed set to: ");
        Serial.println(value);
    } else {
        analogWrite(_pwmPin, 0); // Set PWM value to 0
        digitalWrite(_relayPin, LOW); // Turn off the relay
        Serial.print(_id); // Print the pump identifier
        Serial.println(" is OFF");
    }
}
