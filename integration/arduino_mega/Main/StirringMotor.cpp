/*
 * StirringMotor.cpp
 * This file provides the implementation of the StirringMotor class defined in StirringMotor.h.
 * The class controls a stirring motor (fan) using PWM signals.
 */

#include "StirringMotor.h"

// Constructor for StirringMotor
StirringMotor::StirringMotor(int pwmPin, int relayPin, const char* id)
    : _pwmPin(pwmPin), _relayPin(relayPin), status(false), _id(id) {
    pinMode(_pwmPin, OUTPUT);
    pinMode(_relayPin, OUTPUT);
}

// Method to control the stirring motor
void StirringMotor::control(bool state, int value) {
    if (state && value > 0) {
        int pwmValue = rpmToPWM(value); // Calculate corresponding PWM value for the target RPM
        analogWrite(_pwmPin, pwmValue); // Apply the PWM value to the motor
        digitalWrite(_relayPin, HIGH);  // Turn on the relay
        status = true;                  // Set the status to on
        Serial.print("Stirring Motor is ON, RPM set to: ");
        Serial.print(value);
        Serial.print(" corresponds to PWM value: ");
        Serial.println(pwmValue);
    } else {
        analogWrite(_pwmPin, 0);       // Set PWM value to 0
        digitalWrite(_relayPin, LOW);  // Turn off the relay
        status = false;                // Set the status to off
        Serial.println("Stirring Motor is OFF");
    }
}

// Method to check if the motor is on
bool StirringMotor::isOn() const {
    return status;
}

// Method to convert RPM to PWM value
int StirringMotor::rpmToPWM(int targetRPM) {
    float loadPercentage; // Variable to hold the calculated load percentage

    // Calculate the load percentage based on the target RPM using piecewise linear equations
    if (targetRPM <= 450) {
        // First equation for RPMs from 0% to 32% load
        loadPercentage = (targetRPM - 390) / 1.875;
    } else {
        // Second equation for RPMs from 32% to 100% load
        loadPercentage = (targetRPM - 450) / 15.441 + 32;
    }

    // Convert the load percentage to a PWM value (range 0 to 255)
    int pwmValue = map(int(loadPercentage), 0, 100, 0, 255);
    return pwmValue;
}
