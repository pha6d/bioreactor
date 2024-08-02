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
     * @param isPWMCapable: Boolean indicating if the hardware supports PWM control.
     * @param name: Identifier for the heating plate (for debugging purposes).
     */
    HeatingPlate(int relayPin, bool isPWMCapable, const char* name);

    /*
     * Method to initialize the heating plate.
     * This method is called to set up the heating plate before it's first used.
     */
    void begin() override;

    /*
     * Method to control the heating plate.
     * @param state: Boolean indicating whether the heating plate should be on or off.
     * @param value: Power percentage (0-100) if hardware is PWM capable, ignored otherwise.
     */
    void control(bool state, int value = 0) override;

    /*
     * Method to check if the heating plate is on.
     * @return Boolean indicating if the heating plate is on.
     */
    bool isOn() const override;

    /*
     * Method to get the name of the heating plate.
     * @return The name of the heating plate.
     */
    const char* getName() const override { return _name; }

    /*
     * Method to control the heating plate using PID output.
     * @param pidOutput: The output from the PID controller (0-255).
     */
    void controlWithPID(double pidOutput);

private:
    int _relayPin;   // Relay or PWM pin
    const char* _name;
    bool status;     // Track the state of the heating plate
    bool _isPWMCapable; // Indicates if the hardware supports PWM

    // For cycle control (non-PWM hardware)
    unsigned long cycleTime = 10000; // 10 seconds per cycle = In process chemistry, it offers a good compromise between control accuracy and relay life. It is not necessary to increase it in most cases.
    unsigned long lastCycleStart = 0;
    double dutyCycle = 0; // 0 to 1

    void controlPWM(int value);
    void controlOnOff(bool state);
    void controlWithCycle(double percentage);
    int _lastPercentPower = 0;
};

#endif