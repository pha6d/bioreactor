/*
 * DCPump.h
 * This file defines a class for controlling DC pumps using PWM, a relay, and a motor speed controller.
 * The class implements the ActuatorInterface and provides methods to control the pump with a specified
 * minimum PWM value. This is useful for controlling various types of DC pumps, such as air pumps and liquid drain pumps.
 *
 * Physical modules used:
 * - Relay module
 * - PW1VA01 PWM to Voltage converter
 * - Motor speed controller
 * - DC pump (air pump or liquid drain pump)
 */

#ifndef DCPUMP_H
#define DCPUMP_H

#include "ActuatorInterface.h"
#include <Arduino.h>

class DCPump : public ActuatorInterface {
public:
    /*
     * Constructor for DCPump.
     * @param pwmPin: The pin connected to the PWM input of the pump.
     * @param relayPin: The pin connected to the relay controlling the pump's power.
     * @param minPWM: The minimum PWM value required to run the pump.
     * @param id: Identifier for the pump (for debugging purposes).
     */
    DCPump(int pwmPin, int relayPin, int minPWM, const char* id);

    /*
     * Method to control the pump.
     * @param state: Boolean indicating whether the pump should be on or off.
     * @param value: Integer value to control the speed of the pump.
     */
    void control(bool state, int value);

private:
    int _pwmPin;    // PWM pin
    int _relayPin;  // Relay pin
    int _minPWM;    // Minimum PWM value
    const char* _id; // Identifier for the pump
};

#endif
