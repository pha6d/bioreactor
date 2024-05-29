/*
 * LEDGrowLight.h
 * This file defines a class for controlling a LED grow light.
 * The class implements the ActuatorInterface and provides methods to turn the LED grow light on or off.
 *
 * Physical modules used:
 * - Relay module connected to the LED grow light
 *
 * The 2835 LED chip emits light in the red and blue spectrum, which are crucial for plant growth. 
 * Red light (around 660 nm) helps in flowering and fruit production, while blue light (around 450 nm) is essential for vegetative growth.
 */

#ifndef LEDGROWLIGHT_H
#define LEDGROWLIGHT_H

#include "ActuatorInterface.h"
#include <Arduino.h>

class LEDGrowLight : public ActuatorInterface {
public:
    /*
     * Constructor for LEDGrowLight.
     * @param relayPin: The pin connected to the relay controlling the LED grow light.
     * @param id: Identifier for the LED grow light (for debugging purposes).
     */
    LEDGrowLight(int relayPin, const char* id);

    /*
     * Method to control the LED grow light.
     * @param state: Boolean indicating whether the LED grow light should be on or off.
     * @param value: Not used in this implementation.
     */
    void control(bool state, int value = 0);

private:
    int _relayPin;   // Relay pin
    const char* _id; // Identifier for the LED grow light
};

#endif
