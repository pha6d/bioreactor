/*
 * HeatingPlate.h
 * This file defines a class for controlling a heating plate.
 * The class supports both on/off control and PWM control for future hardware.
 */

#ifndef HEATINGPLATE_H
#define HEATINGPLATE_H

#include "ActuatorInterface.h"
#include <Arduino.h>

#define MAX_SAFE_TEMPERATURE 45.0

class HeatingPlate : public ActuatorInterface {
public:
    /*
     * Constructor for HeatingPlate.
     * @param relayPin: The pin connected to the relay or PWM pin for the heating plate.
     * @param id: Identifier for the heating plate (for debugging purposes).
     * @param isPWMCapable: Boolean indicating if the hardware supports PWM control.
     */
    HeatingPlate(int relayPin, const char* id, bool isPWMCapable = false);

    /*
     * Method to control the heating plate.
     * @param state: Boolean indicating whether the heating plate should be on or off.
     * @param value: PWM value (0-255) if hardware is PWM capable, ignored otherwise.
     */
    void control(bool state, int value = 0) override;

    /*
     * Method to check if the heating plate is on.
     * @return Boolean indicating if the heating plate is on.
     */
    bool isOn() const override;

    const char* getName() const override { return _id; }

    /*
     * Method to control the heating plate using PID output.
     * @param pidOutput: The output from the PID controller (0-255).
     */
    void controlWithPID(double pidOutput);

private:
    int _relayPin;   // Relay or PWM pin
    const char* _id; // Identifier for the heating plate
    bool status;     // Track the state of the heating plate
    bool _isPWMCapable; // Indicates if the hardware supports PWM

    // For cycle control (non-PWM hardware)
    unsigned long cycleTime = 10000; // 10 seconds per cycle
    unsigned long lastCycleStart = 0;
    double dutyCycle = 0; // 0 to 1

    void controlPWM(int value);
    void controlOnOff(bool state);
    void controlWithCycle(double pidOutput);
    int _lastPercentPower = 0;
};

#endif