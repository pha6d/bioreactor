/*
 * StirringMotor.h
 * This file defines a class for controlling a stirring motor using PWM.
 * The class implements the ActuatorInterface and provides methods to control
 * the motor speed based on a target RPM.
 *
 * Physical modules used:
 * - PWM fan (e.g., be quiet! Pure Wings 2 PWM 120 mm 1500rpm (4 pins))
 * - Arduino PWM pin
 * - Relay module
 */

// The tachometer pin of the fan could not be used because the voltage was too low in the state to be read by arduino.

#ifndef STIRRINGMOTOR_H
#define STIRRINGMOTOR_H

#include "ActuatorInterface.h"
#include <Arduino.h>

class StirringMotor : public ActuatorInterface {
public:
    /*
     * Constructor for StirringMotor.
     * @param pwmPin: The pin connected to the PWM control wire of the fan.
     * @param relayPin: The pin connected to the relay controlling the fan's power.
     */
    StirringMotor(int pwmPin, int relayPin);

    /*
     * Method to control the stirring motor.
     * @param state: Boolean indicating whether the motor should be on or off.
     * @param value: Integer value to control the target RPM of the motor.
     */
    void control(bool state, int value) override;

    /*
     * Method to check if the motor is on.
     * @return Boolean indicating if the motor is on.
     */
    bool isOn() const override;

private:
    int _pwmPin;   // PWM pin
    int _relayPin; // Relay pin
    bool status;   // Track the state of the motor

    /*
     * Method to convert RPM to PWM value.
     * @param targetRPM: The desired RPM for the motor.
     * @return: Corresponding PWM value.
     */
    int rpmToPWM(int targetRPM);
};

#endif
