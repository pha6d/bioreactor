/*
 * AirFlowSensor.h
 * This file defines a class for reading the air flow rate from a YF-S401 air flow meter.
 * The class implements the SensorInterface and provides methods to initialize the sensor and read the flow rate.
 *
 * Physical modules used:
 * - YF-S401 Air Flow Meter
 */

#ifndef AIRFLOWSENSOR_H
#define AIRFLOWSENSOR_H

#include "SensorInterface.h"
#include <Arduino.h>

class AirFlowSensor : public SensorInterface {
public:
    /*
     * Constructor for AirFlowSensor.
     * @param pin: The digital pin connected to the air flow meter's signal wire.
     */
    AirFlowSensor(int pin);

    /*
     * Method to initialize the air flow meter sensor.
     */
    void begin();

    /*
     * Method to read the flow rate from the sensor.
     * @return: The flow rate in liters per minute (L/min).
     */
    float readValue();

    /*
     * Interrupt service routine to count pulses.
     */
    static void countPulses();

private:
    int _pin; // Digital pin connected to the air flow meter's signal wire
    volatile unsigned long _pulseCount; // Pulse count from the air flow meter
    unsigned long _lastTime; // Last time the flow rate was calculated
    static const unsigned long _interval = 1000; // Interval to calculate flow rate (in milliseconds)
    static const float _pulsesPerLiter; // Pulses per liter as per the sensor's specification

    static AirFlowSensor* instance; // Static instance for the interrupt handler
};

#endif
